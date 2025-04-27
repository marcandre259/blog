---
layout: math
title: Getting maths to render on this blog
date: 2025-04-27
---

# Going insane trying to get math to render on this blog
I realized last week that mathematical equations would not render when writing
on this blog. I had a wild ride trying to get this working already, but now the time
has come to get those equations to render.

By rendering math, I mean getting LaTeX syntax to show up as nice, high
resolution pictures or scalable vector graphics. This blog post will end with
such graphics.

## Methodology
My approach is to give the google's Gemini 2.5 Flash large language model (LLM)
as much context as possible about the issue. Namely, I give:
- The structure of the blog's project, so the folders and files within.
- That I am using github pages to deploy the blog.
- What is in the _config.yml file of my blog.
- The issue itself, namely that I'd like to see something like $\frac{x}{2}$
  rendered properly (if it does not show up as mangled LaTeX, the mission was successful).

As a sidenote, I started using the Pro and Flash iterations of the Gemini 2.5
model last week and really like them. At the moment, the experimental versions
of the model are free as in free beer.

## Taking the LLM to heart
The first recommendation of the LLM is to create a `default.html` layout that
overrides the basic *minima* layout of the blog.

To do this, I copy and paste the main blog page to default.html and add some
instructions to import *mathjax*. *mathjax* is the package that should render
the maths.

Doing this destroys the blog's styling and does not render the maths.

## It works on the LLM's computer
The approach is to instead extend the default *minima* style of the blog. So I
get rid of `default.html` and create a `math.html` file instead. In that file, I
include a kind of markdown layer where I specify that the layout is *default*.

```html
---
layout: default
---

<!-- Add this MathJax script -->
<script type="text/javascript" async
    src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.7/MathJax.js?config=TeX-MML-AM_CHTML">
    </script>
<!-- End of MathJax script -->
```

The LLM assures me that it tried this solution and that it worked. It does not. The blog's style is back, but the math is not rendering.

As a check, I put the *mathjax* import script directly in this blog post.

This also did not work. In a further query, the LLM recommended I include
*mathjax* explicitly in my `_config.yml`.

```yaml
title: Tapestry of flimsy steps
author: Marc-André Chénier
theme: minima

kramdown:
  math_engine: mathjax
  syntax_highlighter: rouge
  input: GFM # Optional: Use GitHub Flavored Markdown
```

I had good hopes, but this approach also failed. The next suggestion is to sandwich LaTeX expressions in a
*raw* html routine. Again, without success.

## Other attempts
Here is an inline fraction:
\(\frac{x}{2}\).

Here is a display equation:
\[ a^2 + b^2 = c^2 \]

This should be the one...

$$
\int x^2y \delta x
$$

## At last
What got things rolling is finding out about [csega's
blog](https://github.com/Csega/csega.github.io) blog. He has a post on there
where he makes a similarly heroic attempt to render $\frac{x}{2}$. Since he's
also using *jekyll* as his site generator, it gave me confidence this feat is
possible.

I then went ahead and install *jekyll* locally so I could serve and quickly debug the website from my laptop. This can be done with

```bash
jekyll serve
```

I had some issues getting *jekyll* to compile but most were solved with a variation of:

```bash
gem install ...
```

*Gem* is *Ruby*'s package manager. With this set-up, debugging went a lot
faster. I also went ahead and *vibe-coded* the rest with *cline*.

*Cline* is an extension for the *VS Code* editor. It gives you a chat interface
for LLMs, along with some tools to read and edit your project files. Very much
like *Cursor*, except that you can set it up with an API of your choice without
having to pay a subscription.

On the one hand, I could have spent this sunny afternoon reading jekyll's and
github pages documentation at length instead of iterating with an LLM. On the other hand, some blockers are nice to just push aside with *vibe-coding*.
