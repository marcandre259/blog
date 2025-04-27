---
title: Getting maths to render on this blog
layout: math
date: 2025-04-27
---

# Going insane trying to get maths to render on this blog
I realized last week that mathematical equations would not render when writing
on this blog. I had a wild ride trying to get this working already, but now time
has come to get those equations to render.

By rendering maths, I mean getting LaTeX syntax to show up as nice, high
resolution pictures or scalable vector graphics. This blog post will end with
such graphics.

## Methodology
My approach is to give the google's Gemini 2.5 Flash large language model (LLM)
as much context as possible about the issue. Namely, I give:
- The structure of the blog's project, so the folders and files within.
- That I am using github pages to deploy the blog.
- What is in the _config.yml file of my blog.
- The issue itself, namely that I'd like to see something like $\frac{x}{2}$
  rendered properly.

As a sidenote, I started using the Pro and Flash iterations of the Gemini 2.5
model last week and really like them. At the moment, the experimental versions
of the model are free as in free beer.

## Taking the LLM to heart
The first recommendation of the LLM is to create a `default.html` layout that
overides the basic *minima* layout of the blog.

To do this, I copy and paste the entry blog page to default.html and add some
instructions to import *mathjax*. *mathjax* is the package that should renders
the maths.

Doing this destroys the blog's styling and does not render the maths.
