---
layout: default
title: Tapestry of flimsy steps
---

## Blog Posts

<ul>
  {% for post in site.posts %}
    <li>
      <a href="{{ post.url | relative_url }}">{{ post.title | escape }}</a>
      <time datetime="{{ post.date | date_to_xmlschema }}">{{ post.date | date: "%b %d, %Y" }}</time>
    </li>
  {% endfor %}
</ul>
