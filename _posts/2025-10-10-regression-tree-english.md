---
layout: math
title: Regression tree algorithm from scratch in C (English)
date: 2025-10-09
---

This post is translated from Dutch with Claude 4.5 Sonnet. I did reread and revise the translation.

## Motivation
In this blog post I describe a basic regression tree algorithm in C. Regression and decision trees are found everywhere in the machine learning landscape today. Even in 2025 and despite the emergence of pre-trained one-shot transformer models, gradient boosting machines (GBM) are often the best methods for making good predictions (see e.g. W. Rizkallah, Journal of Big Data, 2025). In industry I also frequently encounter algorithms like LightGBM that are variations on the famous GBM.

Of all the components that make up GBMs and their various implementations, the regression or decision tree is the most important. On top of other innovations and components like loss functions, feature binning, gradient descent, one-side sampling, these trees form the foundation of GBMs and related algorithms like the Random Forest.
The difference between regression and decision lies in the outcome the analyst tries to predict. If the outcome is discrete: yes or no, with two or more categories, then you're dealing with a decision tree. If the outcome is continuous, house prices for example, then you're dealing with a regression tree.

Essentially both trees use the same fitting strategy. In a defined number of steps, they split a data sample into a number of leaves so that the difference between each leaf's values and its related outcomes is smallest. Now, if you simply want to reduce the difference between outcomes and sample leaves, you'd find it best to define one leaf per observation. But then the tree model has no generalizing power. So concessions must be made toward generalization. This happens by setting parameters like the minimum number of data per leaf, the maximum depth of the tree, or the minimum gain that's allowed.

## Structure
The intention behind this post is that you can learn how regression trees work by writing the algorithm yourself in C. So I'm going to provide the necessary functions without putting them together. I think that's an enjoyable way to learn something. I'm certainly no C expert myself, so watch out because there will be memory leaks in the code. I pay no attention whatsoever to the leaks. This is certainly not production-ready code.

I'll try to convey the intuition and logic behind each code snippet. To a certain extent.
The focus remains on the implementation and not on the intuition, so I recommend those who need more intuition to find it on [scikit-learn](https://scikit-learn.org/stable/auto_examples/tree/plot_tree_regression.html#sphx-glr-auto-examples-tree-plot-tree-regression-py).

## Useful concepts 
The implementation uses a good number of concepts that come from programming and statistics domains.

To find the best splits between leaves the data must be sorted according to the order of magnitude of each variable. So it's necessary to use a sorting algorithm.
To be able to decide whether a split is beneficial, we use so-called gradients, hessians and a gain formula. In this case, the gradients are more or less centered outcomes, and the hessians are unit vectors.

Recursion will be used frequently. The tree itself is built by a recursive algorithm. To be able to use the fitted trees to make predictions you also have to traverse them with recursion. Tree traversal is also used to calculate the feature importance. One can say that a regression tree gives a path to each observation (or instance in machine learning language...), and that this path must be followed through recursion. So recursion will play a very important role below.

To be able to test the algorithm, I also use the simple random number generator (RNG) from the C standard library. Data simulation is thus also a present concept.

Then there are a number of things associated with using C. Namely: pointers and memory addresses, stack and heap, structs and data types. I only have a practical understanding of these concepts and will therefore try to explain them without depth. The most important thing there is to know that a dynamic array in C is a pointer to the first memory address of the array's data. The analyst must therefore always keep good track of how long that array is. Otherwise you get garbage.

Also important for recursive functions is the difference between data that sits on the stack and data that sits on the heap. Data on the stack is removed as soon as it's outside the current scope of the program. Data on the heap is the opposite. It's kept until it's released by the analyst. So heap data can be changed in a function and that changed data can be used within the scope of a second function. That's often necessary with recursion.

## Ingredients

It's good to start with the small number of ingredients that will be used. The first keeps info about a possible split. It's defined like so:

```C
typedef struct SplitInfo {
  float gain; 
  float threshold; 
} SplitInfo;
```
This SplitInfo struct must retain data about the best split of each feature. Each split has a threshold, through which observations whose value on that feature is smaller than the threshold are given to the so-called left children node, and the rest to the right children node.

The gain is retained for two reasons. First to be able to compare the best split of different features. Second, each split gain can be used to calculate feature importance.

The tree itself is a collection of related nodes. Each node has a depth. At depth zero one finds the root node that contains the entire dataset. At depth one this root node is divided into two children nodes. Each node gets its own set of observations. Then each of those two nodes at depth one is again divided in two and so on. At the final depth are the leaves. These leaf nodes give predictions based on the preceding splits. The regression tree is actually a binary tree.

Each node is defined like so:

```C
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
```

The left and right Nodes are pointers to children nodes. Because each Node contains its own children one can say it's a recursive object. That's why I use pointers for the left and right nodes. Otherwise a Node object would have an infinite number of children and grandchildren nodes.

A Node also has a *feature\_id* and a *threshold*, to retain on which feature and at which value the data was split. To know if the tree is large enough, I also keep the depth of the node.
If a node is a leaf, it gets a value of true on *is\_leaf*. Otherwise the node gets a value of false there. Leaves get a *value*, but no *feature\_id* or *threshold* since they have no children nodes.

Finally I define the *RegressionTree* struct:


```C
typedef struct RegressionTree {
  int max_depth;
  int min_leaf_samples;
  float constant;
  Node *root;

} RegressionTree;
```

The tree contains the root node, two complexity constraints and a constant. From the root node you can traverse the entire tree. The complexity constraints are used to sacrifice bias to improve generalization. In short, because I want to be able to make predictions on new data with the model.

The constant is the mean of the outcome values. By subtracting the constant from the outcomes, I get the gradients. This may seem like a detour but will actually help with the calculation of the gains.

A final struct I use is the *MaskIndices*:

```C
typedef struct MaskIndices {
  int *left_indices;
  size_t left_n;
  int *right_indices;
  size_t right_n;
} MaskIndices;
```

Once you find a split threshold, you'll want to know which of those observations must go to the left and which to the right children nodes. This data is retained by the left and right indices. With C it's complicated to calculate the quantity of components of a dynamic array, so I also take along *left\_n* and *right\_n*.

## Functions

### Main function

Alright, finally we encounter the content. Let me start with the end of this project: the main function. It contains all important steps, from the declaration of a simulated dataset and a regression tree model to the discovery of the feature importances.

```C
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
```

To start I create a dataset with three features X and an outcome Y. The features lie within the double pointer X. From that double pointer the three pointer columns are accessible. We're going to play a lot with the pointers since they're necessary to build dynamic arrays like X and Y in C.

To give values to X I take samples from a uniform distribution. *rand* returns a value between 0 and RAND_MAX. So by dividing by RAND_MAX you get a value between 0 and 1. Good enough to test.

Y is completely dependent on the first and second column of X. So when I look at the feature importances, I should see a zero importance value for the third column of X.

```C
  RegressionTree *reg_tree = malloc(sizeof(RegressionTree));

  reg_tree->max_depth = 3;
  reg_tree->min_leaf_samples = 5;


  fit(reg_tree, X, Y, n, m);

  float *predictions = predict(reg_tree, X, n, m);
```

I then declare a *RegressionTree* on the heap, so that I can fit it later with *fit*. *fit* is the most important part of this program and is responsible for splitting X into a number of leaves.

*predict* then gives a prediction for each observation in X. How good is the average prediction? That can be checked through the mean squared error (mse).


```C
  float mse_model = mse_compute(Y, predictions, n);

  printf("MSE model: %.3f\n", mse_model);

  float *mean_y_vector = (float *)malloc(sizeof(float) * n);

  for (int i = 0; i < n; i++) {
    mean_y_vector[i] = mean_y;
  }

  float mse_null = mse_compute(Y, mean_y_vector, n);

  printf("MSE null: %.3f\n", mse_null);
```

I do an evaluation by comparing the mse of the tree model with that of a so-called null model. In this case the null model is the mean of all Y values. In other words, a tree with a single node.

```C
  float *feature_importances = compute_feature_importance(reg_tree, m);

  for (int i = 0; i < m; i++) {
    printf("Feat importance %d: %.3f\n", i, feature_importances[i]);
  }
```

Compared to other machine learning or AI methods, trees are fairly transparent. By summing the gain of each split one can get a translation of which variables are the most important according to the model.

Now that we've taken a bird's-eye view over each step of the program, let's look at how each part of it is built.


## Sorting
Sorting the observations is necessary to be able to calculate a gain for each potential threshold. Because I want to use the order of a variable to sort multiple arrays, I use a function that returns the sorting index instead of the sorted input array *arr*.

```C
size_t *arg_sort(float *arr, size_t n) {
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
```

To sort I use the *bubblesort* algorithm because I find it easy to remember. C has its own sorting function in the standard library, but I don't think it or an alternative function returns the sorting indices.

Another detail is the use of *memcpy*. I do it so that I don't accidentally change the data behind the arr dynamic array.

Once I have the sorting indices, another function, *reorder*, must do the actual sorting.

```C
float *reorder(float *arr, size_t *reorder_indices, size_t n) {
  float *reordered_arr = (float *)malloc(sizeof(float) * n);
  for (int i = 0; i < n; i++) {
    reordered_arr[i] = arr[reorder_indices[i]];
  }

  return reordered_arr;
}
```

## A bit of arithmetic

Throughout the program there are a number of small functions that are responsible for mathematical operations. You've already seen one:

```C
float mse_compute(float *actuals, float *predictions, size_t n) {
  float sse = 0;
  for (int i = 0; i < n; i++) {
    sse += pow(actuals[i] - predictions[i], 2.0);
  }

  return sse/(float)n;
}
```

*mse\_compute* is one of the simpler functions in the program and can be enjoyable for a beginner to play a bit with C. Otherwise it's a very ordinary function and thus requires no explanation.

At the same level of complexity is a useful *mean* function:

```C
float mean(float *arr, size_t n) {
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum/(float)n;
}
```

Now I can talk about the two interesting calculation functions: *compute\_gain* and *compute\_leaf\_value*. Let me start with *compute\_leaf\_value*.

```C
float compute_leaf_value(float G_sum, float H_sum) {
  return -G_sum/H_sum;
}
```

Here the value of a leaf is calculated. *G_sum* is the sum of the gradients in the leaf. Briefly put, here a gradient is the difference between the mean of Y and a Y observation. So if you have say two Y observations in a leaf, the gradients are $\mathrm{mean}(Y)−Y1$
and $\mathrm{mean}(Y)−Y2$. Remember that the hessians always have the value 1 here. Then *compute_leaf_value* simply gives the negative mean gradient of the leaf.

Why do I take the negative? Suppose that $Y_1$ and $Y_2$ are both larger than $\mathrm{mean}(Y)$. Then the gradients will be negative, but if you multiply their sum by -1, you get a leaf value that's positive. And if you add the mean of Y to the leaf value, you precisely get a prediction that sits between $Y_1$ and $Y_2$.

The gradients are thus usable not only to find node splits, but also to make the predictions. In that sense the gradients are reusable.

In *compute\_gain*, I can show how these gradients are used to find a split. This happens by comparing the gains of all possible splits per feature or variable.

```C
float compute_gain(float gradient_left, float gradient_right, float hessian_left, float hessian_right) {
  float left_side = pow(gradient_left, 2.0) / hessian_left + pow(gradient_right, 2.0) / hessian_right;
  float right_side = pow(gradient_left + gradient_right, 2.0) / (hessian_left + hessian_right);

  return left_side - right_side;
}
```

I'll start with the *right\_side* of the formula. That gives the gain if you don't split the node, but instead set it as a leaf. In other words it's the gain if you don't let the tree grow.

The *left\_side* of the formula looks at the information that's generated by the split. If you think about the formula, you see that it has a recognizable form: $x^2+y^2−(x+y)^2$, which can only be positive if $x$ has the opposite sign of $y$. So the goal of the regression tree is to explain the variation around the mean of Y as well as possible. This is done by greedily revealing the dependency of Y with X.

## Searching for the correct fit

The fit function is relatively simple. It finds the mean of outcome Y, defines the gradients and hessians dynamic arrays, allocates memory to the root node and calls the recursive *\_split\_node* function.

```C
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
```

## The art of the split

*\_split\_node* is a thick function with a large number of arguments and multiple conditional statements. I'll describe it briefly and then dump the entire function below. The goal of that summary is to give structure to an attentive reading of *\_split\_node*.

I first look at whether a split can be made. If the tree has reached its *max\_depth*, or if the split nodes would have a too small number of observations, the function returns the leaf value with *compute\_leaf\_value* (see the last else instruction). The fit of a branch of the tree is then done.

When the opposite happens, that is, when a split can be made, the function navigates through all variables looking for the variable whose split gives a best gain, and its threshold.

Much code is then responsible for preparing the split by sending the correct data to the left and right children nodes. To know which value lies above and which value lies below the threshold, I use the *MaskIndices* struct. If a value lies below the threshold, I send it to the left node. What would happen in two Python lines, sometimes happens with 30 lines of C...

Before I call *\_split\_node* again, I allocate memory for the left and right children nodes. I also ensure that the depth of each child increases by one.

A difference with other functions that are typically used to learn recursion, is that each call to *\_split\_node* returns something. The edge condition of the recursion is *should\_split*.

```C
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
```

The first function within *\_split\_node* that is called is *should\_split*. It looks at a couple of conditions to know if a split is allowed. If yes, there's still a further check on the gain. The gain of the top found split must be positive, otherwise the regression tree without the new split gives a better fit.

I find the *min\_leaf\_samples* check interesting. $\frac{\text{n\_samples}}{2}$ is the minimum size of the largest of the left and right children nodes. The *min\_leaf\_samples* is thus the smallest number of observations in the largest child that a split allows.

```C
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
```

The second called function within *\_split\_node* is find_best_split. It searches for a threshold on an array *arr* that returns the largest gain. That gain gets as arguments the sums of the left and right gradients. The hessians are here only used to normalize the sums.

To minimize the use of data masking, I use the earlier shown *arg\_sort* and *reorder* functions, together with cumulative sums. I can thus easily get the left and right gradient and hessian sums. I'd like to say this is the most efficient way to do things, but since I have to use a *bubblesort*, I don't know. In my opinion, it's certainly easy to read.

```C
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
```

After I've found the best split threshold, I do have to use masking to split the data and send it to the relevant children. I start by deducing the maximum size of each indice, and once I know the actual size of the indice arrays, I call *realloc* to release the unnecessary memory.


```C
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
```

If you've gotten to here, congratulations, you know how to fit a regression tree in C. You'll probably still want to keep reading though. I'm going to talk about two exciting tree traversal functions. These are very useful to get some juice out of the regression tree.


## Two pleasant tree traversals 

I'll start with my favorite of the two: the calculation of the feature importances. These give the importance of each variable of *X*. This is done by finding on which variable or feature each split was done, and adding the gain of that split to a *feature\_importances* array of size $1 \times m$ where $m$ is the number of variables.

```C
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
```

That recursive function is used within the *compute\_feature\_importance* function. That does a bit of bookkeeping.

The second tree traversal is done to allow making predictions. For each observation with values on all variables of X, we want to be able to make a prediction. This happens by finding the path through the splits until the leaf with a prediction for $x$ is reached.

```C
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
```

In short that's done by looking, for each split until a leaf is reached, whether $x$ is below or above the threshold of each relevant split feature. Accordingly, we follow the left or right path, until root->is_leaf is true.

Again, I do a bit of bookkeeping to make and retain predictions for each $x$ within a matrix X:

```C
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
```

## Possible further exercises 

There are a few interesting exercises you can do to learn even more from regression trees.

- Above we've made use of the entire dataset to find a split. As the number of training observations grows larger, that becomes less interesting. Modern tools like LightGBM and XGBoost can make use of histogram binning to handle big data better. In Python it's not so difficult to do this with a few numpy functions like *searchsorted* and *bincount*. So you can try it in Python and then translate the solution to C.

  The idea is to sum the gradients and hessians per bin to then find the split by searching each bin instead of the entire dataset. If you still want to keep finding so-called exact splits you can also look for a solution for duplicate feature values. This can also give efficiency, especially if you're working with a small number of values.

- Another exercise can be to make a random forest or a gradient boosting machine from the regression tree. To a certain extent that's actually simpler than the exercise above.

- Another interesting exercise would be to make a decision tree instead of a regression tree. Even more complicated would be to correctly predict more than two discrete outcomes.
