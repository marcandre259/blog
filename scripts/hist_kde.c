#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct KdeOutput
{
    double *kernel_density;
    double *mu_positions;
} KdeOutput;

double gaussian_kernel(double mu, double sigma, double x)
{
    double exponential_part = pow((x - mu) / sigma, 2) * -0.5;
    double constant_part = 1.0 / (sqrt(2.0 * M_PI) * sigma);

    return exp(exponential_part) * constant_part;
}

KdeOutput kde(double *data, int data_size, int num_bins, double sigma)
{
    double min = data[0];
    double max = data[0];

    for (int i = 0; i < data_size; i++)
    {
        if (data[i] < min)
        {
            min = data[i];
        }
        if (data[i] > max)
        {
            max = data[i];
        }
    }

    // Getting the width of the bins
    double bin_width = (max - min) / num_bins;

    double *mu_positions = (double *)malloc(num_bins * sizeof(double));
    double *kernel_density = (double *)calloc(num_bins, sizeof(double));

    // Iterating over the mu positions and then the data points
    double *ptr_mu = mu_positions;
    double *ptr_density = kernel_density;
    for (int j = 0; j < num_bins; j++)
    {
        *ptr_mu = min + bin_width * (j + 0.5);
        // Pointing to first density address
        for (int i = 0; i < data_size; i++)
        {
            // Weight increment at memory address
            *ptr_density += gaussian_kernel(*ptr_mu, sigma, data[i]);
        }

        // Normalization step
        *ptr_density /= data_size * sigma;

        ptr_mu++;
        ptr_density++;
    }

    KdeOutput kde_output;

    kde_output.kernel_density = kernel_density;
    kde_output.mu_positions = mu_positions;

    return kde_output;
}

void free_kde_output(KdeOutput kde_output)
{
    free(kde_output.kernel_density);
    free(kde_output.mu_positions);
}

int main()
{
    double data[] = {10.0, 11.0, 12.5, 12.2, 16.0};

    int data_size = (int)(sizeof(data) / sizeof(data[0]));
    int num_bins = 10;

    KdeOutput kde_output = kde(data, data_size, num_bins, 1.0);

    for (int i = 0; i < num_bins; i++)
    {
        printf("Weight: %.3f\n", kde_output.kernel_density[i]);
        printf("Location: %.3f\n", kde_output.mu_positions[i]);
    }

    free_kde_output(kde_output);

    return 0;
}
