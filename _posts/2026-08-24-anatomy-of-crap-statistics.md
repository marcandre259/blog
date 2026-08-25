---
layout: math
title: Rencontre avec un `causal collider` dans la savane scientifique.
date: 2026-08-23
---

Voilà [un mauvais article](https://pmc.ncbi.nlm.nih.gov/articles/PMC11979035/), une victime appropriée sur l'autel de la pensée critique. C'est aisé à constater, et il y a un paquet de trucs qui frappent dès qu'on le survole.

En premier lieu, la division des sports en plusieurs dizaines de catégories arbitraires. Celles-ci sont ensuite utilisées pour produire un modèle statistique par sport. À vue d'œil, je compte environ 40 catégories qui reçoivent chacune un effet sur l'espérance de vie calculé séparément. Les différences estimées entre les sports aiguisent le scepticisme. Chez les hommes, la gymnastique augmenterait l'espérance de vie de 8,2 ans, mais le volleyball la diminuerait de 5,4 ans. Ces différences vont bien au-delà de la précision indiquée par les intervalles de confiance.

Entre autres éléments à problème, je donne une mention d'honneur à l'exclusion des athlètes encore en vie. Il suffit de trouver un sport qui a gagné en popularité durant les dernières décennies pour comprendre le problème. Si le sport a effectivement pour effet d'augmenter l'espérance de vie, alors seuls les athlètes les plus vulnérables seront déjà morts. Les autres seront toujours en vie. Si on compare ce groupe d'athlètes décédés à la population générale, on peut alors estimer que le sport diminue l'espérance de vie.

Mais le problème auquel je veux donner plus de temps risque d'être répété par des scientifiques dont l'éducation statistique est correcte. Les auteurs de *Sport and longevity* utilisent l'année de décès pour construire leur variable `age delta`. Ils prennent l'âge au décès d'un athlète, puis soustraient l'espérance de vie à la naissance pour son sexe et son pays durant l'année de son décès.

C'est une comparaison qui m'a surpris. Ce genre d'analyse contrôle habituellement plutôt pour la date de naissance. On évite ainsi de comparer les pratiquants d'un sport récent avec des individus nés beaucoup plus tôt, à une époque où l'espérance de vie était moins grande.

Si on contrôle plutôt pour la date de décès comme le font les auteurs, on contrôle sur un *collider* ([McElreath, *Statistical Rethinking*, chap. 6](https://xcelab.net/rm/statistical-rethinking/)).

Prenons le volleyball, qui gagne en popularité après son entrée aux Jeux olympiques en 1964. Les nouveaux athlètes ont alors autour de 20 ans. Ils sont donc nés vers 1944 et pratiquent plus souvent ce sport que les cohortes précédentes. Parmi les personnes mortes en 1980, ces volleyeurs sont morts vers 36 ans, tandis qu'une personne née en 1900 est morte à 80 ans. En contrôlant pour l'année de décès, on associe donc le volleyball à une mort plus jeune puisque ses pratiquants viennent surtout de cohortes plus récentes que celles des autres personnes mortes la même année.

Contrôler pour la date de naissance permet plutôt de comparer des personnes de la même cohorte. Le biais créé par la date de décès peut être représenté sur un *Directed Acyclic Graph* (DAG).

![DAG reliant la date de naissance, le sport, l'âge au décès et la date de décès](/blog/assets/anatomy-of-crap-statistics-dag.svg)

- `N` est la date de naissance.
- `S` est la pratique du sport.
- `A` est l'âge au décès.
- `Y` est la date de décès.

La date de naissance influence le sport pratiqué. Elle influence aussi l'âge au décès puisque l'espérance de vie change avec les cohortes. La date de naissance et l'âge au décès déterminent ensemble la date de décès.

Il n'y a pas de flèche de `S` vers `A` puisque le sport n'a aucun effet causal dans cet exemple. L'exclusion des athlètes encore en vie n'est pas représentée dans ce DAG. C'est un biais séparé.

`Y` est donc un collider entre `N` et `A`. Le chemin `N -> Y <- A` est fermé tant qu'on ne contrôle pas pour `Y`. Ce contrôle ouvre le chemin `S <- N -> Y <- A`. Il ne ferme pas le chemin `S <- N -> A` qui confond déjà l'effet du sport avec celui de la cohorte de naissance.

Prenons deux personnes mortes en 1980. Une personne née en 1900 est morte à 80 ans. Une personne née en 1944 est morte à 36 ans. À date de décès égale, la personne née le plus tard est nécessairement morte plus jeune. Si les cohortes récentes pratiquent plus souvent un sport, le contrôle pour la date de décès associe ce sport à une mort plus jeune.

Je trouve utile de démontrer avec une simulation. Je simule 5 000 personnes nées entre 1930 et 1989. La probabilité de pratiquer le sport augmente avec l'année où elles atteignent 20 ans. L'année 1964 sert de point de repère. Les cohortes récentes vivent aussi plus longtemps. La pratique du sport n'entre pas dans le calcul de l'âge au décès. Son effet causal est nul.

```python
import numpy as np

rng = np.random.default_rng(42)

birth_date = np.sort(rng.choice(np.arange(1930, 1990), size=5_000))
career_year = birth_date + 20

sport_eta = -1.0 + 0.15 * (career_year - 1964)
sport_prob = 1 / (1 + np.exp(-sport_eta))
does_sport = rng.binomial(1, sport_prob)

age_at_death = (
    65
    + 0.25 * (birth_date - birth_date.min())
    + rng.normal(0, 8, size=5_000)
)
year_at_death = birth_date + age_at_death
```

J'ajuste d'abord un modèle de l'âge au décès en fonction du sport. J'ajoute ensuite l'année de décès au même modèle.

| Modèle | Effet moyen du sport | Intervalle à 90 % |
|---|---:|---:|
| Sans l'année de décès | +6,43 ans | 6,0 à 6,8 |
| Avec l'année de décès | -5,39 ans | -5,8 à -5,0 |

Le premier modèle confond l'effet du sport avec celui de la cohorte. Les cohortes récentes pratiquent plus souvent le sport et vivent plus longtemps. L'ajout de l'année de décès ne corrige pas ce problème. Il renverse l'association. À année de décès égale, les membres des cohortes récentes sont morts plus jeunes. Comme ils pratiquent aussi plus souvent le sport, le deuxième modèle attribue au sport un effet négatif de 5,39 ans.
