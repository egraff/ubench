#ifndef UBENCH_H
#define UBENCH_H

#include <stddef.h>
#include <stdint.h>


typedef uint64_t ubench_time_t;


struct ubench_clock;
struct ubench_runner_params;


typedef void (*ubench_alg_bench_runner_fn)(
    struct ubench_runner_params *params
);


struct ubench_clock_cls
{
    void (* const clock_init)(struct ubench_clock *clock);
};


struct ubench_clock
{
    void (*clock_warmup)(struct ubench_clock *clock);
    ubench_time_t (*clock_start)(struct ubench_clock *clock);
    int64_t (*clock_stop)(struct ubench_clock *clock, ubench_time_t start_time);
    void (*clock_deinit)(struct ubench_clock *clock);

    int clock_is_initialized;
    void *clock_data;
};


struct ubench_lib
{
    void (* const lib_init)(void **lib_data);
    void (* const lib_deinit)(void *lib_data);
};


struct ubench_alg
{
    void (* const alg_before)(
        const void *lib_data,
        const void *alg_input_params,
        void **alg_data
    );
    void (* const alg_run)(
        const void *lib_data,
        const void *alg_input_params,
        const void *alg_data
    );
    void (* const alg_after)(
        const void *lib_data,
        const void *alg_input_params,
        void *alg_data
    );
};


struct ubench
{
    struct ubench_clock clock;
    const struct ubench_lib *lib;
    void *lib_data;
};


struct ubench_runner_params
{
    int64_t (*run_alg)(
        const void *alg_input_params,
        size_t num_iters,
        size_t num_warmup_iters
    );
    int64_t (*determine_overhead)(
        size_t num_samples,
        size_t sample_size,
        size_t num_iters,
        int64_t *sample_min_values
    );
};


extern const struct ubench_clock_cls * const ubench_default_clock_cls;
extern const struct ubench_clock_cls * const ubench_cycle_clock_cls;


void
ubench_init(struct ubench *ub);


void
ubench_use_lib(struct ubench *ub, const struct ubench_lib *lib);


void
ubench_use_clock(struct ubench *ub, const struct ubench_clock_cls *clock_cls);


void
ubench_deinit(struct ubench *ub);


void
ubench_run_alg_bench(
    struct ubench *ub,
    const struct ubench_alg *alg,
    ubench_alg_bench_runner_fn alg_bench_runner
);

#endif /* UBENCH_H */
