---
layout: math
title: A brief C (and Python) adventure in density estimation
date: 2025-04-28
---

# A brief C (and Python) adventure in density estimation

## The histogram and the kernel density estimator

### Motivating plots
Usually, the histogram is defined by its look or by saying it's a way to
plot a continuous sample of data, e.g. some array of numbers [2.0, 4.0, 2.5,
...]. You could use an histogram to report on exam scores for example. Plots,
like the histogram and kernel density estimator (kde), are great tools to
summarize a sample of data while still keeping a lot of information about that
raw sample.

Think of an exam you took for example, you may start with some questions like:
- Am I above average?
- To which bracket of my peers do I belong, the top 20%, the top 50%?
- What proportion of students failed or passed the exam?
- How does the distribution of score look like, was the exam a pass or fail
  ordeal, or was there a mass of students who barely passed or failed?

In a complete analysis, you'd want a clear and explicit to those questions that
you find important. That means computing the quantile, or the mean, some
proportions, etc.. The point of plots like the histogram or the KDE, is to give you an overview of the story. Yes, maybe the mean is what interests you, but isn't nice to know that while you were better than average, you still performed worse than 50% of the class? That's the kind of early hypotheses or insights plots provide.


TODO: What is the density of random value?


