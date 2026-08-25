#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

typedef struct SplitInfo {
  float gain; 
  float threshold; 
} SplitInfo;


typedef struct Node {
  int feature_id;
  float threshold;
  float value;
  int depth;
  struct Node *left;
  struct Node *right;
  bool is_leaf;
  float gain;
} Node;

typedef struct RegressionTree {
  int max_depth;
  int min_leaf_samples;
  float constant;
  Node *root;

} RegressionTree;

size_t *arg_sort(float *arr, size_t n) {
  // Start with the array, and the size 
  // Create an index array, move the indices 
  // in the same direction as the sorted values
  // return the resulting index array

  float *arr_copy = (float *)malloc(sizeof(float) * n);
  memcpy(arr_copy, arr, sizeof(float) * n);

  size_t *index_arr = (size_t *)malloc(sizeof(size_t) * n);
  for (int i = 0; i < n; i++) {
    index_arr[i] = i;
  }

  float next_value;
  size_t next_index;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < (n - i - 1); j++) {
      if (arr_copy[j] > arr_copy[j+1]) {
        next_value = arr_copy[j + 1];
        next_index = index_arr[j + 1];

        arr_copy[j + 1] = arr_copy[j];
        index_arr[j + 1] = index_arr[j];

        arr_copy[j] = next_value;
        index_arr[j] = next_index;
      }
    }
  }

  return index_arr;
}

float mean(float *arr, size_t n) {
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum/(float)n;
}

float compute_gain(float gradient_left, float gradient_right, float hessian_left, float hessian_right) {
  float left_side = pow(gradient_left, 2.0) / hessian_left + pow(gradient_right, 2.0) / hessian_right;
  float right_side = pow(gradient_left + gradient_right, 2.0) / (hessian_left + hessian_right);

  return left_side - right_side;
}

float *reorder(float *arr, size_t *reorder_indices, size_t n) {
  float *reordered_arr = (float *)malloc(sizeof(float) * n);
  for (int i = 0; i < n; i++) {
    reordered_arr[i] = arr[reorder_indices[i]];
  }

  return reordered_arr;
}

SplitInfo find_best_split(float *arr, float *gradients, float *hessians, size_t n) {
  size_t *index_arr = arg_sort(arr, n);
  float *sorted_arr = reorder(arr, index_arr, n);
  float *sorted_gradients = reorder(gradients, index_arr, n);
  float *sorted_hessians = reorder(hessians, index_arr, n);

  float *cumsum_gradients = (float *)malloc(sizeof(float) * n);
  float *cumsum_hessians = (float *)malloc(sizeof(float) * n);

  cumsum_gradients[0] = sorted_gradients[0];
  cumsum_hessians[0] = sorted_hessians[0];
  // Need also the sums to get the left split gains
  float sum_gradients = sorted_gradients[0];
  float sum_hessians = sorted_hessians[0];

  for (int i = 1; i < n; i++) {
    cumsum_gradients[i] = sorted_gradients[i] + cumsum_gradients[i-1];
    cumsum_hessians[i] = sorted_hessians[i] + cumsum_hessians[i-1];

    sum_gradients += sorted_gradients[i];
    sum_hessians += sorted_hessians[i];
  }

  // Calculate gains for each possible split, and find best gain
  float best_gain = 0;
  float best_threshold = 0;

  // Setting i to max n-2 to avoid illegal splits
  for (int i = 0; i < (n-1); i++) {
    float gradient_left = cumsum_gradients[i];
    float gradient_right = sum_gradients - gradient_left; 

    float hessian_left = cumsum_hessians[i];
    float hessian_right = sum_hessians - hessian_left; 

    float gain = compute_gain(gradient_left, gradient_right, hessian_left, hessian_right);

    if (gain > best_gain) {
      best_gain = gain;
      best_threshold = sorted_arr[i];
    }
  }


  SplitInfo split_info;

  split_info.gain = best_gain;
  split_info.threshold = best_threshold;

  return split_info;
}

bool should_split(int depth, int n_samples, int max_depth, int min_leaf_samples) {
  if (
    depth < max_depth && (n_samples / 2) > min_leaf_samples
  ) {
    return true;
  }
  else {
    return false;
  }
}

typedef struct MaskIndices {
  int *left_indices;
  size_t left_n;
  int *right_indices;
  size_t right_n;
} MaskIndices;

MaskIndices split_on_feature_threshold(
  float threshold, 
  int feature_id, 
  float **X, 
  size_t n) {
  int *left_indices = (int *)malloc(sizeof(int) * n);
  int *right_indices = (int *)malloc(sizeof(int) * n);

  int left_cnt = 0;
  int right_cnt = 0;

  for (int i = 0; i < n; i++) {
    if (X[feature_id][i] <= threshold) {
      left_indices[left_cnt] = i;
      left_cnt++;
    }
    else {
      right_indices[right_cnt] = i;
      right_cnt++;
    }
  }

  left_indices = realloc(left_indices, sizeof(int) * left_cnt);
  right_indices = realloc(right_indices, sizeof(int) * right_cnt);

  MaskIndices mask_indices;

  mask_indices.left_indices = left_indices;
  mask_indices.left_n = left_cnt;

  mask_indices.right_indices = right_indices;
  mask_indices.right_n = right_cnt;

  return mask_indices;
}

float compute_leaf_value(float G_sum, float H_sum) {
  return -G_sum/H_sum;
}

Node *_split_node(
  Node *root,
  int max_depth,
  int min_leaf_samples,
  float **X, 
  float *gradients,
  float *hessians,
  size_t m,
  size_t n) {
  int depth = root->depth;

  bool split_decision = should_split(depth, n, max_depth, min_leaf_samples);

  if (split_decision == true) {
    root->is_leaf = false;

    int best_feature_id = 0;
    float best_threshold = 0;
    float best_gain = 0;

    // Need to first find the best split
    for (int j = 0; j < m; j++) {
      SplitInfo split_info = find_best_split(X[j], gradients, hessians, n);
      float current_gain = split_info.gain;
      if (current_gain > best_gain) {
        best_feature_id = j;
        best_threshold = split_info.threshold;
        best_gain = current_gain;
      }
    }

    if (best_gain <= 0) {
      float G_sum = 0;
      float H_sum = 0;

      for (int i = 0; i < n; i++) {
        G_sum += gradients[i];
        H_sum += hessians[i];
      }

      root->value = compute_leaf_value(G_sum, H_sum);
      root->is_leaf = true;
    }
    else { 
      // Saving the gains of each split node to compute feature importance later
      root->gain = best_gain;
      root->threshold = best_threshold;
      root->feature_id = best_feature_id;

      MaskIndices mask_indices = split_on_feature_threshold(
        best_threshold, 
        best_feature_id,
        X, 
        n
      );

      // Make left and right arrays to pass to split_node
      float *left_gradients = (float *)malloc(sizeof(float) * mask_indices.left_n);
      float *left_hessians = (float *)malloc(sizeof(float) * mask_indices.left_n);

      float *right_gradients = (float *)malloc(sizeof(float) * mask_indices.right_n);
      float *right_hessians = (float *)malloc(sizeof(float) * mask_indices.right_n);

      float **X_left = (float **)malloc(sizeof(float *) * m);
      float **X_right = (float **)malloc(sizeof(float *) * m);

      for (int j = 0; j < m; j++) {
        X_left[j] = (float *)malloc(sizeof(float) * mask_indices.left_n);
        X_right[j] = (float *)malloc(sizeof(float) * mask_indices.right_n);
      }

      for (int i = 0; i < mask_indices.left_n; i++) {
        int left_index = mask_indices.left_indices[i];
        left_gradients[i] = gradients[left_index];
        left_hessians[i] = hessians[left_index];

        for (int j = 0; j < m; j++) {
          X_left[j][i] = X[j][left_index];
        }
      }

      for (int i = 0; i < mask_indices.right_n; i++) {
        int right_index = mask_indices.right_indices[i];
        right_gradients[i] = gradients[right_index];
        right_hessians[i] = hessians[right_index];

        for (int j = 0; j < m; j++) {
          X_right[j][i] = X[j][right_index];
        }
      }
      printf("Split: left_n=%zu, right_n=%zu\n", mask_indices.left_n, mask_indices.right_n);

      // Define left and right nodes before recursive calls
      Node *left_node = (Node *)malloc(sizeof(Node));
      left_node->is_leaf = true;
      left_node->depth = depth + 1;

      Node *right_node = (Node *)malloc(sizeof(Node));
      right_node->is_leaf = true;
      right_node->depth = depth + 1;

      root->left = _split_node(
        left_node,
        max_depth,
        min_leaf_samples,
        X_left,
        left_gradients,
        left_hessians,
        m,
        mask_indices.left_n
      );

      root->right = _split_node(
        right_node,
        max_depth,
        min_leaf_samples,
        X_right,
        right_gradients,
        right_hessians,
        m,
        mask_indices.right_n
      );
    }
  }

  // If the split is not acceptable
  else {
    float G_sum = 0;
    float H_sum = 0;

    for (int i = 0; i < n; i++) {
      G_sum += gradients[i];
      H_sum += hessians[i];
    }

    root->value = compute_leaf_value(G_sum, H_sum);
  }
  // Now got to mask the data according to feature and threshold

  return root;
}

void fit(RegressionTree *reg_tree, float **X, float *Y, size_t n, size_t m) {
  float mean_y = mean(Y, n);

  // Assign mean_y as constant of tree (for predictions)
  reg_tree->constant = mean_y;

  float *gradients = (float *)malloc(sizeof(float) * n);
  float *hessians = (float *)malloc(sizeof(float) * n);

  for (int i = 0; i < n; i++) {
    gradients[i] = mean_y - Y[i];
    hessians[i] = 1.0; 
  }

  reg_tree->root = (Node *)malloc(sizeof(Node));
  Node *root = reg_tree->root;

  root->depth = 0;
  root->is_leaf = true;

  _split_node(
    root, 
    reg_tree->max_depth, 
    reg_tree->min_leaf_samples, 
    X, 
    gradients, 
    hessians, 
    m, 
    n);
}

float _predict_single(Node *root, float *x, float constant) {
  // x is a 1 by m array

  if (root->is_leaf == false) {
    if (x[root->feature_id] <= root->threshold) {
      return _predict_single(root->left, x, constant);
    }
    else {
      return _predict_single(root->right, x, constant);
    }
  }
  else {
    return root->value + constant;
  }
}

float *predict(RegressionTree *reg_tree, float **X, size_t n, size_t m) {
  float *predictions = (float *)malloc(sizeof(float) *n);

  float constant = reg_tree->constant;

  for (int i = 0; i < n; i++)  {
    float *x = (float *)malloc(sizeof(float) * m);
    // Prep the prediction vector
    for (int j = 0; j < m; j++) {
      x[j] = X[j][i];
    }

    float value = _predict_single(reg_tree->root, x, constant);

    free(x);

    predictions[i] = value;
  }

  return predictions;
}

float mse_compute(float *actuals, float *predictions, size_t n) {
  float sse = 0;
  for (int i = 0; i < n; i++) {
    sse += pow(actuals[i] - predictions[i], 2.0);
  }

  return sse/(float)n;
}

void free_nodes(Node *root) {
  if (root == NULL) {
    return;
  }

  free_nodes(root->left);
  free_nodes(root->right);

  free(root);
}

void _feature_importance(Node *node, float *feature_importances) {
  if (node->is_leaf == true) {
    return;
  }

  else {
    feature_importances[node->feature_id] += node->gain;

    _feature_importance(node->left, feature_importances);
    _feature_importance(node->right, feature_importances);
  }
}

float *compute_feature_importance(RegressionTree *reg_tree, size_t m) {
  float *feature_importances = (float *)malloc(sizeof(float) * m);

  // Assign 0 start value
  for (int j = 0; j < m; j++) {
    feature_importances[j] = 0;
  }

  Node *root = reg_tree->root;

  _feature_importance(root, feature_importances);

  // normalize the importances
  float G_sum = 0;
  for (int j = 0; j < m; j++) {
    G_sum += feature_importances[j];
  }

  for (int j = 0; j < m; j++) {
    feature_importances[j] = feature_importances[j] / G_sum;
  }

  return feature_importances;
}



int main() {
  srand(time(NULL));

  size_t n = 1000;
  size_t m = 3;
  float **X = (float **)malloc(sizeof(float *) * m);
  X[0] = (float *)malloc(sizeof(float) * n);
  X[1] = (float *)malloc(sizeof(float) * n);
  X[2] = (float *)malloc(sizeof(float) * n);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      X[j][i] = (float)rand() / RAND_MAX;
    }
  }

  // Assign a y array, with a simple relation to the x values
  float *Y = (float *)malloc(sizeof(float) * n);

  for (int i = 0; i < n; i++) {
    Y[i] = X[0][i] * 0.2 + X[1][i] * 0.5;
  }

  float mean_y = mean(Y, n);

  // printf("Mean y: %.3f\n", mean_y);

  RegressionTree *reg_tree = malloc(sizeof(RegressionTree));

  reg_tree->max_depth = 3;
  reg_tree->min_leaf_samples = 5;


  fit(reg_tree, X, Y, n, m);

  //Node *left_node = reg_tree->root->left;

  // printf("%d\n", left_node->is_leaf);

  // // Try a prediction
  // float *x = (float *)malloc(sizeof(float) * m);
  // x[0] = 50;
  // x[1] = 50;

  // float prediction = _predict_single(reg_tree->root, x, mean_y);

  float *predictions = predict(reg_tree, X, n, m);

  // for (int i = 0; i < n; i++) {
  //   printf("Prediction: %.3f\n", predictions[i]);
  // }

  float mse_model = mse_compute(Y, predictions, n);

  printf("MSE model: %.3f\n", mse_model);

  float *mean_y_vector = (float *)malloc(sizeof(float) * n);

  for (int i = 0; i < n; i++) {
    mean_y_vector[i] = mean_y;
  }

  float mse_null = mse_compute(Y, mean_y_vector, n);

  printf("MSE null: %.3f\n", mse_null);

  float *feature_importances = compute_feature_importance(reg_tree, m);

  for (int i = 0; i < m; i++) {
    printf("Feat importance %d: %.3f\n", i, feature_importances[i]);
  }


  // printf("Pred: %3.f\n", prediction);

  free(feature_importances);

  free(X[0]);
  free(X[1]);
  free(X);

  free(Y);

  free_nodes(reg_tree->root);

  free(reg_tree);


  return 0;

}


