#include "ubench_priv.h"
#include "ubench_compiler.h"

#include <ubench/ubench.h>
#include <string.h>


struct ubench_alg_run
{
    struct ubench *ub;
    const struct ubench_alg *alg;
};


extern const struct ubench_alg ubench_empty_alg;
static thread_local const struct ubench_alg_run *current_alg_run;


static int64_t
run_alg(
    struct ubench *ub,
    const struct ubench_alg *alg,
    const void *alg_input_params,
    size_t num_iters,
    size_t num_warmup_iters
)
{
    int64_t start, elapsed;
    size_t  i;
    void   *alg_data = NULL;

    if (alg->alg_before)
    {
        alg->alg_before(ub->lib_data, alg_input_params, &alg_data);
    }

    for (i = 0; i < num_warmup_iters; i++)
    {
        alg->alg_run(ub->lib_data, alg_input_params, alg_data);
    }

    if (ub->clock.clock_warmup)
    {
        ub->clock.clock_warmup(&ub->clock);
    }

    start = ub->clock.clock_start(&ub->clock);
    for (i = 0; i < num_iters; i++)
    {
        alg->alg_run(ub->lib_data, alg_input_params, alg_data);
    }
    elapsed = ub->clock.clock_stop(&ub->clock, start);

    if (alg->alg_after)
    {
        alg->alg_after(ub->lib_data, alg_input_params, alg_data);
    }

    return elapsed;
}


static int64_t
run_alg_cb(
    const void *alg_input_params,
    size_t num_iters,
    size_t num_warmup_iters
)
{
    struct ubench *ub = current_alg_run->ub;
    const struct ubench_alg *alg = current_alg_run->alg;

    return run_alg(ub, alg, alg_input_params, num_iters, num_warmup_iters);
}


static int64_t
determine_overhead(
    size_t num_samples,
    size_t sample_size,
    size_t num_iters,
    int64_t *sample_min_values
)
{
    struct ubench *ub = current_alg_run->ub;
    int64_t elapsed_min, sample_elapsed, sample_elapsed_min;
    size_t i, s;

    elapsed_min = 0;
    for (s = 0; s < num_samples; s++)
    {
        sample_elapsed_min = 0;
        for (i = 0; i < sample_size; i++)
        {
            sample_elapsed = run_alg(
                ub,
                &ubench_empty_alg,
                NULL,
                num_iters,
                0
            );

            if ((i == 0) || (sample_elapsed < sample_elapsed_min))
            {
                sample_elapsed_min = sample_elapsed;
            }
        }

        if (sample_min_values)
        {
            sample_min_values[s] = sample_elapsed_min;
        }

        if ((s == 0) || (sample_elapsed_min < elapsed_min))
        {
            elapsed_min = sample_elapsed_min;
        }
    }

    return elapsed_min;
}


static void
deinit_lib(struct ubench *ub)
{
    if (ub->lib)
    {
        ub->lib->lib_deinit(ub->lib_data);
    }

    ub->lib = NULL;
    ub->lib_data = NULL;
}


static void
deinit_clock(struct ubench *ub)
{
    if (ub->clock.clock_is_initialized)
    {
        if (ub->clock.clock_deinit)
        {
            ub->clock.clock_deinit(&ub->clock);
        }
    }

    memset(&ub->clock, 0, sizeof(ub->clock));
}


void
ubench_init(struct ubench *ub)
{
    memset(ub, 0, sizeof(*ub));

    ubench_use_lib(ub, NULL);
    ubench_use_clock(ub, NULL);
}


void
ubench_use_lib(struct ubench *ub, const struct ubench_lib *lib)
{
    void *lib_data = NULL;

    deinit_lib(ub);

    if (lib)
    {
        lib->lib_init(&lib_data);
    }

    ub->lib = lib;
    ub->lib_data = lib_data;
}


void
ubench_use_clock(struct ubench *ub, const struct ubench_clock_cls *clock_cls)
{
    deinit_clock(ub);

    if (clock_cls == NULL)
    {
        clock_cls = ubench_default_clock_cls;
    }

    ubench_assert(clock_cls->clock_init != NULL);

    clock_cls->clock_init(&ub->clock);

    /* Verify that clock_init() populated required function pointers */
    ubench_assert(ub->clock.clock_start != NULL);
    ubench_assert(ub->clock.clock_stop != NULL);

    ub->clock.clock_is_initialized = 1;
}


void
ubench_deinit(struct ubench *ub)
{
    deinit_lib(ub);
    deinit_clock(ub);

    memset(ub, 0, sizeof(*ub));
}


void
ubench_run_alg_bench(
    struct ubench *ub,
    const struct ubench_alg *alg,
    ubench_alg_bench_runner_fn alg_bench_runner
)
{
    struct ubench_alg_run alg_run;
    struct ubench_runner_params runner_params;

    memset(&alg_run, 0, sizeof(alg_run));
    alg_run.ub = ub;
    alg_run.alg = alg;

    memset(&runner_params, 0, sizeof(runner_params));
    runner_params.run_alg = run_alg_cb;
    runner_params.determine_overhead = determine_overhead;

    current_alg_run = &alg_run;
    alg_bench_runner(&runner_params);
}
