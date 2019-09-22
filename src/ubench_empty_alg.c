#include <ubench/ubench.h>


static void
empty_alg_run(
    const void *lib_data,
    const void *alg_input_params,
    const void *alg_data
)
{
    (void)lib_data;
    (void)alg_input_params;
    (void)alg_data;
}


const struct ubench_alg ubench_empty_alg =
{
    .alg_before = NULL,
    .alg_run = empty_alg_run,
    .alg_after = NULL,
};
