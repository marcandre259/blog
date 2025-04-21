## Plan
- Why is it important to know what flow matching is?
- What is a flow?
- Learning the marginal distribution
- Training a model
- Difference with diffusion models (score matching)
- Analogy with diffusion models


## Notes
- Important from papers such as
-


## Execution
In 2024, flow matching was the approach behind the training of state
of the arts generative models such as Flux and Stable Diffusion 3.5.

Flow matching describes matching a flow function to generate samples
from some data distribution $p_{\text{data}}(z)$. $z$ is a datapoint
on a $\mathbb{R}^d$ plane. $z$ can, for example, be an image with
values 256 $\times$ 256 over 3 RBG channels. In that case, $d$ would have value $256 \times 256 \times 3$. $p_{\text{data}}$ is
then the probability distribution of images such as $z$. It does
not have an analytical definiton, but can be approximated by
training a neural network with flow matching on a dataset of interest. For example, images of lions at a watering point.

The flow function itself is a distribution $p_t(x)$ with $t \in [0, 1]$
and $x$ is a point on a path between an initial $\epsilon$ and the
datapoint $z$. In practice, this path is defined by a so-called
noise scheduler. It is simplest to rely here on the linear scheduler where
$x = \alpha_t \cdot z + \beta_t \cdot \epsilon$ with $\alpha_t=t$
and $\beta_t=1-t$. At $t=0$, we have $x = \epsilon$ and at $t=1$, we have $x = z$.

The generation process starts by sampling $\epsilon$ from
$p_\text{init}(\epsilon)$. Of course, $z$ must be generated from
 $\epsilon$ during inference, and there is then no known path between
 some $\epsilon$ and $z$, so that path has to be approximated. The set on $\epsilon \in \mathbb{R}^d$ of the paths to be approximated is the flow function between $p_\text{init}(\epsilon)$ and $p_\text{data}(z)$. We are getting closer.

Since $p_\text{data}(z)$ is unknown, we simplify the problem by
starting from a single datapoint $z$ and giving it a Dirac
distribution. A Dirac distribution has $p(z)=1$ if $Z=z$ and
$p(z)=0$ otherwise. We are then looking for the flow function between
$p_\text{init}(\epsilon)$ and a datapoint $z$. Because this flow function is conditional on a single datapoint $z$, it is called the conditional flow function.

With the continuity theorem, it can be shown that a flow function over
the whole data distribution $p_\text{data}(z)$ can be obtained by taking
samples of $Z$ and averaging the conditional flow functions over these samples. This means that a neural network can be trained
against samples of $Z$ to approximate $p_\text{data}(z)$.

A first idea would be to train the neural network with samples
of $\epsilon$ as inputs and data samples $z$ as output. This is not
as stupid as it sounds if the matches $z$ and $\epsilon$ are maintained,
but it gives a very difficult approximation problem to the neural
network. To facilitate training, we start by considering simulating
the paths between $\epsilon$ and $z$ with a series a steps defined by
a vector field.

-- TODO: Got to add conditioning

To simulate the flow, we use the Euler method:
$$
x_{t+h} = x_t + h \cdot u_t(x_t)
$$
over $n$ steps where $h=\frac{1}{n}$, $t \in [0, 1]$ and $u_t(\cdot)$ is
the vector field. Recall that we defined the flow function as
$x = \alpha_t \cdot z + \beta_t \cdot \epsilon$. We obtain $u_t$ by taking its derivative over $t$:
 $$
u_t(x_t) = \dot{\alpha}_t \cdot z + \dot{\beta}_t \cdot \epsilon
 $$

TODO: Don't need the part below I think, just need to do the z-e conversion
which we express in terms of $x_t$ by plugging in
$$
\epsilon = \frac{(x_t - \alpha_t \cdot z)}{\beta_t}
$$
giving
$$
u_t(x_t) = \left(\dot{\alpha}_t - \frac{\dot{\beta}_t}{\beta_t}\alpha_t\right)\cdot z + \frac{\dot{\beta}_t}{\beta_t} x_t
$$






