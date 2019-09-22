#include <ubench/ubench.h>

#include <limits.h>


#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201102L) && !defined(__STDC_NO_THREADS__)
#  include <threads.h>
#endif

#if !defined(thread_local)
#  if defined(_MSC_VER)
#    define thread_local __declspec(thread)
#  elif defined(__GNUC__)
#    define thread_local __thread
#  endif
#endif

#if !defined(thread_local)
#  error "No thread_local support"
#endif


struct ubench_alg_run
{
    const struct ubench *ub;
    const struct ubench_alg *alg;
};


extern const struct ubench_alg ubench_empty_alg;
static thread_local const struct ubench_alg_run *current_alg_run;


static int64_t
run_alg(
    const struct ubench *ub,
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

    ub->clock->clock_warmup(ub->clock_data);

    start = ub->clock->clock_start(ub->clock_data);
    for (i = 0; i < num_iters; i++)
    {
        alg->alg_run(ub->lib_data, alg_input_params, alg_data);
    }
    elapsed = ub->clock->clock_stop(ub->clock_data, start);

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
    const struct ubench *ub = current_alg_run->ub;
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
    const struct ubench *ub = current_alg_run->ub;
    int64_t elapsed_min, sample_elapsed, sample_elapsed_min;
    size_t i, s;

    elapsed_min = 0;
    for (s = 0; s < num_samples; s++)
    {
        sample_elapsed_min = 0;
        for (i = 0; i < sample_size; i++)
        {
            sample_elapsed = run_alg(ub, &ubench_empty_alg, NULL, num_iters, 0);
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


void
ubench_init(
    struct ubench *ub,
    const struct ubench_clock *clock,
    const struct ubench_lib *lib
)
{
    void *clock_data = NULL;
    void *lib_data = NULL;

    if (clock->clock_init)
    {
        clock->clock_init(&clock_data);
    }

    if (lib)
    {
        lib->lib_init(&lib_data);
    }

    ub->clock = clock;
    ub->clock_data = clock_data;
    ub->lib = lib;
    ub->lib_data = lib_data;
}


void
ubench_deinit(struct ubench *ub)
{
    if (ub->lib)
    {
        ub->lib->lib_deinit(ub->lib_data);
    }

    if (ub->clock->clock_deinit)
    {
        ub->clock->clock_deinit(ub->clock_data);
    }

    ub->clock = NULL;
    ub->clock_data = NULL;
    ub->lib = NULL;
    ub->lib_data = NULL;
}


void
ubench_run_alg_bench(
    const struct ubench *ub,
    const struct ubench_alg *alg,
    ubench_alg_bench_runner_fn alg_bench_runner
)
{
    struct ubench_alg_run alg_run;
    struct ubench_runner_params runner_params;

    alg_run.ub = ub;
    alg_run.alg = alg;

    runner_params.run_alg = run_alg_cb;
    runner_params.determine_overhead = determine_overhead;

    current_alg_run = &alg_run;
    alg_bench_runner(&runner_params);
}
