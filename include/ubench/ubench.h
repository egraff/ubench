#ifndef UBENCH_H
#define UBENCH_H

#include <stddef.h>
#include <stdint.h>


struct ubench_runner_params;


typedef void (*ubench_alg_bench_runner_fn)(struct ubench_runner_params *params);


struct ubench_clock
{
    void (*clock_init)(void **clock_data);
    void (*clock_deinit)(void *clock_data);
    void (*clock_warmup)(const void *clock_data);
    int64_t (*clock_start)(void *clock_data);
    int64_t (*clock_stop)(void *clock_data, int64_t start_val);
};


struct ubench_lib
{
    void (*lib_init)(void **lib_data);
    void (*lib_deinit)(void *lib_data);
};


struct ubench_alg
{
    void (*alg_before)(
        const void *lib_data,
        const void *alg_input_params,
        void **alg_data
    );
    void (*alg_run)(
        const void *lib_data,
        const void *alg_input_params,
        const void *alg_data
    );
    void (*alg_after)(
        const void *lib_data,
        const void *alg_input_params,
        void *alg_data
    );
};


struct ubench
{
    const struct ubench_clock *clock;
    void *clock_data;
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


void
ubench_init(
    struct ubench *ub,
    const struct ubench_clock *clock,
    const struct ubench_lib *lib
);


void
ubench_deinit(struct ubench *ub);


void
ubench_run_alg_bench(
    const struct ubench *ub,
    const struct ubench_alg *alg,
    ubench_alg_bench_runner_fn alg_bench_runner
);

#endif /* UBENCH_H */
