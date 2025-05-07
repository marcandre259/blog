---
layout: math
title: Intégrer la recherche web à Claude Desktop sur Linux Mint
date: 2025-05-07
---

# Intégrer la recherche web à Claude Desktop

## Installation de Claude Desktop sur Linux
Dans ce tutoriel, j'installe [Claude Desktop](https://claude.ai/download) sur
une instance de Linux Mint 22.1. Linux Mint est une distribution Linux assez
populaire basee sur Debian.

Il n'y a presentement pas de version officielle de Claude Desktop disponible
pour Linux. L'alternative que j'ai trouve est ce project sur *github*:
[claude-desktop-debian](https://github.com/aaddrick/claude-desktop-debian).
C'est une adaptation de la version Windows et elle est compatible avec
le protocol MCP. Le protocol MCP permet au client Claude d'acceder a des
outils incluant la recherche web que je vais configurer.

![Alt](/assets/claude_mcp/claude_desktop_linux.png)

Pour faire l'installation de cette adaptation du client Claude, il faut
avoir acces au programme en ligne de commande *git*. Pour installer *git*, il suffit de taper
```bash
apt get install git
```
dans son terminal (en sachant inclure l'access administratif *sudo* si necessaire).

Ensuite, il faut cloner le depot git de [claude-desktop-debian](), puis lancer
le *build* script pour generer un paquet *.deb*. Pour ce faire, j'execute cette
commande dans mon repertoire *~/Documents/*:

```bash
# Clone this repository
git clone https://github.com/aaddrick/claude-desktop-debian.git
cd claude-desktop-debian

# Build the package (Defaults to .deb and cleans build files)
./build.sh
```

Le *build* script est bien foutu et s'occupe d'installer les depedences
necessaires avant l'installation du client Claude lui-meme. Ce processus prend
quelques minutes, assez pour se preparer un cafe.

Le *build* va generer un fichier semblable a
*claude-desktop_{version_number}_{architecture}.deb* dans le repertoire
*claude-desktop-debian*. Dans mon cas, le fichier *.deb* est
*claude-desktop_0.9.3_amd64.deb*. Pour installer le paquet, je commence par executer
```bash
chmod +x claude-desktop_0.9.3_amd64.deb
```
ce qui fait du paquet un fichier executable. Ensuite, j'installe le paquet avec la commande
```bash
apt get install claude-desktop_0.9.3_amd64.deb
```

Une fois l'installation completer, le client Claude devrait pouvoir etre lancer a partir du terminal avec
```bash
claude-desktop
```
Pour tester, je m'identifie dans l'application avec mon compte google et j'envois une petite requete comme *Hello* dans le menu de discussion.

## Configurer un premier serveur MCP
Au lancement, le client Claude Desktop analyse le fichier
*~/.config/Claude/claude_desktop_config.json* pour decouvrir d'eventuels outils.
Lorsque decouverts, ces outils peuvent etre utiliser par le Large Language Model
(LLM) pour repondre aux requetes de l'utilisateurs.

Par example, un outil meteo permet au LLM de consulter la meteo sur internet.
Pour s'assurer que ces outils sont bien utilises, il faut etre suffisemment explicite
dans la formulation de ses requetes. Par exemple, demander *Utilise l'outil
meteo pour donner la meteo d'aujourd'hui* a la place de *Donne la meteo
d'aujourd'hui*. Dans le second cas, le LLM a moins de chance de consulter
l'outil et va alors donner une reponse comme: *mon contexte ne me donne pas
acces aux informations quotidiennes de meteo*.


Initiallement, le fichier *claude_desktop_config.json* n'existera probablement
pas, donc il va falloir le creer avec:
```bash
~/.config/Claude/claude_desktop_config.json
```

Pour se familiariser avec la configuration des serveurs MCP, je recommande de
configurer le serveur *filesystem*. Le serveur *filesystem* comporte des outils
permettant de lire, creer et modifier des fichiers sur votre systeme local. Le
client Claude va toujours demander la permission avant d'utiliser un outil et il
est necessaire de lire ces demandes de permission pour eviter des gros soucis.

Cette partie du tutoriel est tiree de
[MCP-Quickstart](https://modelcontextprotocol.io/quickstart/user). Il suffit
alors de copier colle le texte suivant dans le fichier
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
fichiers. Ils doivent donc etre adapter autant que necessaire. Dans mon cas,
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

Ce texte de configuration etant inclue au fichier *claude_desktop_config.json*,
il faut s'assurer d'avoir la dependence *node js* sur votre systeme. Cela est
necessaire pour l'installation et le lancement des serveur MCP utilisant la
commande *npx*. Pour un distro de type Debian comme Linux Mint, le plus simple
est d'executer ce [script](https://nodejs.org/en/download) dans son terminal:

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

Cela fait, je ferme et reouvre Claude Desktop en executant
```
claude-desktop
```

Je peux alors tester le serveur filesystem en demandant *What folders are in my
Documents*, et apres avec donner les permissions demander, j'obtient la reponse:
*There's one folder in your Documents directory called "claude-desktop-debian".

## Ajouter le serveur brave-search pour la recherche brave
Ce test etant reussie. On peut maintenant passer au moteur de recherche. Pour la recherche j'utilise le serveur brave-search etant donne que son installation et utilisation est relativement simple. Il utilise aussi la meme dependence node js.

La premiere etape est de generer une cle api sur le site web de Brave.
Pour ce faire, il faut creer un compte utilisateur sur brave search api.
Par la suite, le menu API keys permet


Apres avoir inserer la cle et editer la config, consulter un json validator checker (faites attentions de ne pas inclure toute la cle api) sur internet pour verifier que le fichier config n'a pas derreur de syntaxes.

Cela fait, on ferme et reouvre claude-desktop une nouvelle fois. Je pose maintenant cette question
"Search online what is the weather today"

Si la configuration est correcte, le client Claude va demander d'utiliser l'outil brave_web_search avant de donner une reponse plus ou moins valide.






## Notes
- Environmment d'installation

Linux mint 22.1

Premier defis est qu'il n'y pas de version officiel de Claude Desktop pour les distributions Linux.

On va donc installer une version austentivement windows.

Le project est aaddrick/claude-desktop-debian

Le project supporte l'acces aux outils MCP, super!

Necessaire d'installer git

Doit suivre les instructions d'installation pour une distribution de type Debian.

Dans mon cas: la version et l'architecture donne claude-desktop_0.9.3_amd64.deb.

Ne pas oublier donner les permissions executables au fichier .deb avec ```chmod +x```. Autrement, erreur.

Instructions sur le project git, decider d'utiliser sudo apt install directement sur le fichier .deb.


Ensuite claude-desktop peut-etre lancer avec claude desktop a partir du terminal

Apres s'etre enregistrer avec mon compte google, je peux faire un test rapide: j'ecris Hello dans la boite de conversation et je recois "Hello! How can I help you today?" de Claude Dekstop.


```bash
sudo apt install git
```

si ce n'est pas deja le cas.

L'installation des dependences prend quelques minutes. Assez de temps pour se faire un cafe sur un distro nouvellement installe.

Le fichier de configuration, ou les serveurs MCP sont declarer est:
```
~/.config/Claude/claude_desktop_config.json
```

Pour la configuration, j'installe mon editeur de texte prefere: neovim

```
sudo apt install neovim
```

C'est possible de creer le fichier claude_desktop_config.json avec l'ui en allant dans Files->Settings->Developer et en cliquant sur le bouton "Edit Config". On peut aussi tres bien le creer soit meme avec, par exemple,
```
touch ~/.config/Claude/claude_desktop_config.json
```
C'est un fichier vierge de toute facon, que le client Claude consulte pour decouvrir d'eventuels serveurs MCP.

Avant de mettre en place l'outil moteur de recherche, le plus facile est de suivre le tutoriel d'Anthropic pour installer le server-filesystem. C'est un ensemble d'outil permettant au LLM de lire, creer et modifier des fichiers sur notre ordinateur (rester bien prudent).

Faut etre sur de donner le chemin d'acces a nos repertoires.

Pour le test, je donne access a /Home/marc/Documents et /Home/marc/Downloads. Le LLM va alors savoir les utiliser pour acceder a vos fichiers.

Le serveur exige la dependence node js. Pour un distro de type Debian comme Mint, on peut simplement copier et coller le script Download Node.js sur nodejs.org/en/download dans son terminal.

Cela fait, quitter l'application claude desktop (en faisant ctrl-c dans le terminal par exemple) et relancer la.

Maintenant je demande a Claude, What folders are in my Documents, et apres avec
donner les permissions demander, j'botaient la reponse: There's one folder in
your Documents directory called "claude-desktop-debian". Voila, notre LLM a
decouvert l'origine de sa propre existence.

Ce test etant reussie. On peut maintenant passer au moteur de recherche. Pour la recherche j'utilise le serveur brave-search etant donne que son installation et utilisation est relativement simple. Il utilise aussi la meme dependence node js.

La premiere etape est de generer une cle api sur le site web de Brave.
Pour ce faire, il faut creer un compte utilisateur sur brave search api.

Apres avoir inserer la cle et editer la config, consulter un json validator checker (faites attentions de ne pas inclure toute la cle api) sur internet pour verifier que le fichier config n'a pas derreur de syntaxes.

Cela fait, on ferme et reouvre claude-desktop une nouvelle fois. Je pose maintenant cette question
"Search online what is the weather today"

Si la configuration est correcte, le client Claude va demander d'utiliser l'outil brave_web_search avant de donner une reponse plus ou moins valide.


- Claude Desktop -> abonnement ou non
- Protocole MCP
- Brave search set-up
- Test du serveur mcp
- Configuration
- Example
