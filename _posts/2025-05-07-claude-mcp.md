---
layout: math
title: Intégrer la recherche web à Claude Desktop sur Linux Mint
date: 2025-05-07
---

# Intégrer la recherche web à Claude Desktop sur Linux Mint

Cet article explique comment ajouter la recherche web à Claude Desktop sur Linux
en utilisant le protocole MCP (Model Context Protocol), qui permet aux modèles d'IA d'utiliser des outils externes. Cela se fait en trois étapes:
- L'installation du client Claude Desktop sous Linux
- La configuration d'un premier serveur MCP donnant au modèle accès aux fichiers locaux. Cela a des fins de familiarisation.
- La configuration d'un serveur comprenant les fonctionnalités de recherche web.

L'objectif est de permettre au modèle de repondre a des questions avec des informations actuellement disponible en ligne.

- [Intégrer la recherche web à Claude Desktop sur Linux Mint](#intégrer-la-recherche-web-à-claude-desktop-sur-linux-mint)
  - [Installation de Claude Desktop sur Linux](#installation-de-claude-desktop-sur-linux)
  - [Configurer un premier serveur MCP](#configurer-un-premier-serveur-mcp)
  - [Ajouter le serveur brave-search pour la recherche brave](#ajouter-le-serveur-brave-search-pour-la-recherche-brave)

## Installation de Claude Desktop sur Linux
Dans ce tutoriel, j'installe [Claude Desktop](https://claude.ai/download) sur
une instance de Linux Mint 22.1. Linux Mint est une distribution Linux assez
populaire basée sur Debian.

Il n'y a présentement pas de version officielle de Claude Desktop disponible
pour Linux. L'alternative que j'ai trouvée est ce projet sur *github*:
[claude-desktop-debian](https://github.com/aaddrick/claude-desktop-debian).
C'est une adaptation de la version Windows et elle est compatible avec
le protocole MCP. Le protocole MCP permet au client Claude d'accéder à des
outils incluant la recherche web que je vais configurer.

![Alt](/blog/assets/claude_mcp/claude_desktop_linux.png)

Pour installer cette adaptation du client Claude, il faut
avoir accès au programme en ligne de commande *git*. Pour installer *git*, il suffit de taper
```bash
apt get install git
```
dans son terminal (en incluant l'accès administratif *sudo* si nécessaire).

Ensuite, il faut cloner le dépôt git de [claude-desktop-debian](), puis lancer
le *build* script pour générer un paquet *.deb*. Pour ce faire, j'exécute cette
commande dans mon répertoire *~/Documents/*:

```bash
# Clone this repository
git clone https://github.com/aaddrick/claude-desktop-debian.git
cd claude-desktop-debian

# Build the package (Defaults to .deb and cleans build files)
./build.sh
```

Le *build* script est bien fait et s'occupe d'installer les dépendances
nécessaires avant l'installation du client Claude lui-même. Ce processus prend
quelques minutes, assez pour se préparer un café.

Le *build* va générer un fichier semblable à
*claude-desktop_{version_number}_{architecture}.deb* dans le répertoire
*claude-desktop-debian*. Dans mon cas, le fichier *.deb* est
*claude-desktop_0.9.3_amd64.deb*. Pour installer le paquet, je commence par exécuter
```bash
chmod +x claude-desktop_0.9.3_amd64.deb
```
ce qui fait du paquet un fichier exécutable. Ensuite, j'installe le paquet avec la commande
```bash
apt get install claude-desktop_0.9.3_amd64.deb
```

Une fois l'installation complétée, le client Claude devrait pouvoir être lancé à partir du terminal avec
```bash
claude-desktop
```
Pour tester, je m'identifie dans l'application avec mon compte google et j'envoie une petite requête comme *Hello* dans le menu de discussion.

## Configurer un premier serveur MCP
Au lancement, le client Claude Desktop analyse le fichier
*~/.config/Claude/claude_desktop_config.json* pour découvrir d'éventuels outils.
Lorsque découverts, ces outils peuvent être utilisés par le Large Language Model
(LLM) pour répondre aux requêtes de l'utilisateur.

Par exemple, un outil météo permet au LLM de consulter la météo sur internet.
Pour s'assurer que ces outils sont bien utilisés, il faut être suffisamment explicite
dans la formulation de ses requêtes. Par exemple, demander *Utilise l'outil
météo pour donner la météo d'aujourd'hui* à la place de *Donne la météo
d'aujourd'hui*. Dans le second cas, le LLM a moins de chance de consulter
l'outil et va alors donner une réponse comme: *mon contexte ne me donne pas
accès aux informations quotidiennes de météo*.


Initialement, le fichier *claude_desktop_config.json* n'existera probablement
pas, donc il va falloir le créer avec:
```bash
~/.config/Claude/claude_desktop_config.json
```

Pour se familiariser avec la configuration des serveurs MCP, je recommande de
configurer le serveur *filesystem*. Le serveur *filesystem* comporte des outils
permettant de lire, créer et modifier des fichiers sur votre système local. Le
client Claude va toujours demander la permission avant d'utiliser un outil et il
est nécessaire de lire ces demandes de permission pour éviter des gros soucis.

Cette partie du tutoriel est tirée de
[MCP-Quickstart](https://modelcontextprotocol.io/quickstart/user). Il suffit
alors de copier coller le texte suivant dans le fichier
*claude_desktop_config.json*

```json
{
  "mcpServers": {
    "filesystem": {
      "command": "npx",
      "args": [
        "-y",
        "@modelcontextprotocol/server-filesystem",
        "/Users/username/Desktop",
        "/Users/username/Downloads"
      ]
    }
  }
}
```

Les arguments */Users/username/Desktop/* et */Users/username/Downloads/* fournissent
les portes d'acces que peut utiliser le LLM pour chercher et modifier nos
fichiers. Ils doivent donc être adapter autant que nécessaire. Dans mon cas,
j'ai la configuration suivante:

```json
{
  "mcpServers": {
    "filesystem": {
      "command": "npx",
      "args": [
        "-y",
        "@modelcontextprotocol/server-filesystem",
        "/home/marc/Documents/",
      ]
    }
  }
}
```

Ce texte de configuration étant inclut au fichier *claude_desktop_config.json*,
il faut s'assurer d'avoir la dépendance *node js* sur votre système. Cela est
nécessaire pour l'installation et le lancement des serveurs MCP utilisant la
commande *npx*. Pour une distribution de type Debian comme Linux Mint, le plus simple
est d'exécuter ce [script](https://nodejs.org/en/download) dans son terminal:

```bash
# Download and install nvm:
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# in lieu of restarting the shell
\. "$HOME/.nvm/nvm.sh"

# Download and install Node.js:
nvm install 22

# Verify the Node.js version:
node -v # Should print "v22.15.0".
nvm current # Should print "v22.15.0".

# Verify npm version:
npm -v # Should print "10.9.2".
```

Une fois cela fait, je ferme et réouvre Claude Desktop en exécutant
```
claude-desktop
```

Je peux alors tester le serveur filesystem en demandant *What folders are in my
Documents*, et après avoir donné les permissions demandées, j'obtiens la réponse:
*There's one folder in your Documents directory called "claude-desktop-debian".

## Ajouter le serveur brave-search pour la recherche brave
Ce test étant réussi. On peut maintenant passer au moteur de recherche. Pour la
recherche j'utilise le serveur brave-search étant donné que son installation et
utilisation est relativement simple. Il utilise aussi la même dépendance *node
js*.

La première étape consiste à générer une clé API sur le site web de Brave. Pour
ce faire, un compte utilisateur est créé sur brave search api. Par la suite, le
bouton *Add API key* du menu API keys permet d'ajouter une clé API au compte.

![Alt](/blog/assets/claude_mcp/api_key_brave_search.png)

Celle-ci est ensuite ajoutée au fichier de configuration
*claude_desktop_config.json*. Le format attendu pour la configuration est donne
sur ce projet [github](https://github.com/modelcontextprotocol/servers/tree/main/src/brave-search).

```json
{
  "mcpServers": {
    "brave-search": {
      "command": "npx",
      "args": [
        "-y",
        "@modelcontextprotocol/server-brave-search"
      ],
      "env": {
        "BRAVE_API_KEY": "YOUR_API_KEY_HERE"
      }
    }
  }
}
```

Pour préserver les outils *filesystem*, il faut adjoindre cette configuration à
celle donnée ci-dessous. Je donne un exemple pertinent au bas de ce tutoriel.

Après avoir inséré la clé et édité la configuration, je consulte un validateur json
sur internet pour vérifier que le fichier config n'a pas
d'erreur de syntaxe. En faisant cela, je m'assure de ne pas inclure une clé API
valide dans le texte vérifié.

Une fois cela fait, on ferme et réouvre *claude-desktop* une nouvelle fois. Je
pose maintenant cette question "Search online what is the weather today". Si la
configuration est correcte, le client Claude va demander d'utiliser l'outil
brave_web_search avant de donner une réponse plus ou moins valide.

![Alt](/blog/assets/claude_mcp/weather_today.png)

Si cela fonctionne, félicitations! Dans le cas contraire, voici un exemple pour référence:

```json
{
  "mcpServers": {
    "filesystem": {
      "command": "npx",
      "args": [
        "-y",
        "@modelcontextprotocol/server-filesystem",
        "/Users/marc/"
      ]
    },
    "brave-search": {
      "command": "npx",
      "args": [
        "-y",
        "@modelcontextprotocol/server-brave-search"
      ],
      "env": {
        "BRAVE_API_KEY": "BSA7Krtfakekey"
      }
    }
  }
}
```
