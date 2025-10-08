---
title: Regression tree algorithm van niets in C 
date: 2025-10-05
---

## Wat over te hebben 
- Algemeen motivatie
- Gestudeert concepten                                                                 
- Ingredienten: Node, RegressionTree etc. 
- Volgorde 
- Achtergrond voor elke stap 
- Conclusie: toevoegingen e.g. histogram voor efficientie, gbm en random forest, pruning, plotten... 

## Motivatie
In deze blogpost beschrijf ik een basiek regression tree algorithm in C. Regression en decision trees zijn vandaag overal te vinden in de machine learning landschap.
Nog in 2025 en de optredden van pre-trained one-shot transformer models, zijn gradient
boosting machines (GBM) vaak de beste methode om goede voorspellingen te maken (zie bvb. W.
Rizkallah, Journal of Big Data, 2025). Ook in industrie kom ik vaak tegen algorithmen
als LightGBM die variaties zijn op de beroemd GBM. 

Van alle onderdelen die delen maken van GBM's en hun verschillend implementaties, is de
regression of decision tree het belangrisjte. Boven andere innovatie en componenten zoals losse functies, feature binning, gradient descent, one-side sampling, zitten die trees aan de basis van GBM's en geraleteerd algorithmen zoals die Random Forest. 

Het verschil tussen regression en decision zit in de uitkomst die de analyst probeert te
voorspellen. Is de uitkomst discrete, ja of nee, met twee of meer categorien, dan ben je naast een decision tree. Is de uitkomst continue, huisprijsen bijvoorbeeld, dan ben je naast een regression tree. 

Wezenlijk gebruiken de twee trees hetzelfde fit strategie. In een gedefinierd aantal stapen, delen
zij een data sample in een aantal bladjes (leaves) zodat de verschill tussen einige
bladjes waarde en de geraleteerd uitkomsten het minste verschil hebben. Nu, als je
gewoon het verschil tussen uitkomsten en sample bladjes wil verminderen, zou je beste
een bladjes per datum definieren. Dan heeft de tree model geen generaliserend vermogen.
Dus moet er toegegeven gebeuren in het richting van generalisitie. Deelnamelijk gebeurd dit door het
zetten van parameters zoals het minimum aantal data per bladje, het maximum diepte van
de tree of het minimum gain dat toegelaten is. 

## Structure
Het bedoeling acther dit post is dat jij kan zelf leren hoe regression trees werken door
het algorithm zelf in C te schrijven. Dus ga ik de functie signatures zonder invulling geven, zodat je kan hen zelf invullen. Volgens me is dat een plezant manier om iets te leren. Daarna ga ik de ingevuld functies geven. Ik ben zelf zeker geen C expert, dus let op want er gaan memory leaks in de code zijn.

Ik ga proberen de intuitie en de logika achter elke code snippet te doorgeven. Tot een
zekere hoogte.

De focus bleeft op de implementatie en niet op de intuitie, dus rade ik aan deze die meer intuitie nodig hebben om het te vinden op [scikit-learn](https://scikit-learn.org/stable/auto_examples/tree/plot_tree_regression.html#sphx-glr-auto-examples-tree-plot-tree-regression-py).

## Nuttig concepten
Het implementatie gebruikt een mooie aantal concepten die komen van programmatie en
statistiek domeinen.

Om de beste splits tussen bladjes te vinden moet de data gesoorterd werden volgens het
orde van grote van elke variabel. Het is dus nodig om een sorting algorithm te
gebruiken. 

Om te kunnen beslissen of een split bevorderlijk is, gebruiken we zogenoemd gradients,
hessians en een gain formule. In dit geval, zijn die gradients mins of meer gecentered
uitkomsten, en die hessians zijn unit vectors.

Recursie gaat vaak gebruiken werden. De tree zelf zijn gebouwed door een recursive
algorithm. Om de gefitted trees te kunnen gebruiken om voorspellingen te doen moet je
hen ook met recursie doorkruisen. Tree traversal is ook gebruikt om de feature
importance te berekenen. Men kan zeggen dat een regression tree een weg geeft aan elke
datum, en dat die weg gevolgd moet werden door recursie. Dus recursie ga hierbeneden een heel
belangrijk role spellen.

Om de algorithm te kunnen testen, gebruikt ook het simpel random number generator
(RNG) van het C standard library. Data simulatie is dus ook een aanwezig concept.

Daarna zijn een aantal dingen die geassocierd zijn met het gebruik van C. Namelijk,
pointers en memory adresses, stack en heap, struct's en data types. Ik heb enkel een
praktisch begrijp van die concepten en ga hen vandaar zonder diepte proberen uit te
leggen. Het belangrijkste daar is te weten dat een dynamiek array in C een pointer is
tot het eerste memory adress van het array's data. De analyst moet dus altijd goed
bijhouden hoe lang die array is. Andere krijg jij rommel. 

Ook belangrijk voor recursive
functies is een groot verschil tussen data dat zit op de stack en data dat zit op de
heap. Data op de stack is verwijderen zodra het zit buiten het huidige scope van de
program. Data op de heap is het tegenovergesteld. Het is bijgehoud tot het vrijgelaten
is door de analyst. Dus heap data kan veranderd werden in een functie en die veranderd
data kan binnen de scope van een tweede functie gebruikt werden. Dat is vaak nodig met
recursion.

## Ingredienten

Het is goed om te beginnen met het klein aantal ingredienten die gebruikt zullen werden.
Het eerste bijhoudt het info over een mogelijk split. Het is zo gedefinierd:

```C
typedef struct SplitInfo {
  float gain; 
  float threshold; 
} SplitInfo;
```

Die SplitInfo struct moet gegevens over het beste split van elke feature behouden. Elke split heeft een threshold, waardoor observatie wiens waarde op die feature minder is dan de threshold zijn aan de zoegenoemd left split gegeven, en de rest aan de right split. 

De gain is behouden voor twee reden. Ten erste om de best split van verschillende features te kunnen vergelijken. Ten tweede kan elke split gain gebruiken werden om feature importance te berekenen.

De tree zelf is een verzameling van gerelateerd nodes. Elke node heeft een diepte. Op diepte null vind men het root node die het hele dataset bevaat. Op diepte een is deze root node gedeeld in twee node. Elke deel kreeg zijn eigen observaties. Dan zijn elke van die twee node op diepete een nogmaals in twee gedeeld enzovoort. Op het laatse diepte zitten de bladjes. Die nodes geven voorspellingen op basis van de voorafgaand splitsingen. Het regression tree is eigenlijk een binary tree.


Elke node is dus zo gedefinierd:

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

De left en right Node's zijn pointers naar children nodes. Omdat elke Node omvat zijn eigen kinderen kan men zegen dat het een recursive object is. Daarom gebruik ik pointers voor de left en right. Andere zou een Node een oneindigd aantal kinderen en klein-kinderen node hebben.

Een Node heeft ook een feature_id en een treshold, om te behouden op welke feature en op welke waarde was de data gesplit. Om te weten of de tree grote genoeg is, behoud ik ook de depth van de node. 

Als een node een bladjes is, kreeg het een waarde van *true* op *is_leaf*. Andere kreeg daar de node een waarde van *false*. Blajdes kregen ook een *value*, maar geen feature_id of threshold aangezien zij geen kinderen nodes hebben.

Eindelijk definier ik de *RegressionTree* struct:

```C
typedef struct RegressionTree {
  int max_depth;
  int min_leaf_samples;
  float constant;
  Node *root;

} RegressionTree;
```

De tree bevat de root node, twee complexiteit beperkingen en een constant. Van het root node kan je de hele tree doorkruisen. De complexiteit beperkingen zijn gebruikt om iets toe te geven aan generaliteit. In kort omdat ik wil voorspellingen op nieuwe data kunnen doen met de model. 

De constant is het gemiddeld van de uitkomst waarden. Door de uitkomsten van de constant af te trekken, krijg ik de gradients. Dit mag op een omweg te lijken maar ga eigenlijk hulpen met de berekening van de gains. 

Een laatse struct dat ik gebruik is de *MaskIndices*:

```C
typedef struct MaskIndices {
  int *left_indices;
  size_t left_n;
  int *right_indices;
  size_t right_n;
} MaskIndices;
```

Zodra jij een split treshold vind, ga je willen weten welke van die observaties moet naar de linkse en welke naar de rechtse kinderen nodes. Deze gegevens zijn door de links en de rechtse indices behouden. Met *C* is het ingewikkeld om de hoeveelheid onderdelen van een dynamiek array te berekenen, dus neem ik ook *left_n* en *right_n* mee.

## Functies

### Main functie
Oke, eindelijk treffen wij de inhoud. Laat me beginnen met het einde van dit project: het *main* functie.
Die omvaat alle belangrijke stappen, van het declaratie van een gesimuleerd dataset en een regression tree model tot het ontdekking van de feature importances.

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

Om te beginnen creer ik een dataset met drie features X en een uitkomst Y. De features zitten binnen het dubbel pointer X. Van die dubbel pointer zijn de drie pointer columns toegankelijk. We gaan veel met de pointers spelen aangezien ze nodig zijn om dynamic arrays zoals X en Y te bouwen in C.

Om warde te geven aan X neem ik samples van een uniform distributie. *rand* geeft een waarde tussen 0 en *RAND_MAX* terug. Dus door te delen met RAND_MAX kreeg jij een waarde tussen 0 en 1. Goed genoeg om te testen. 

Y is compleet afhankelijk van het eerste en tweede column van X. Dus, als ik keek naar de feature importances, zou ik een null importance waarde zien voor het derde column van X.

```C
  RegressionTree *reg_tree = malloc(sizeof(RegressionTree));

  reg_tree->max_depth = 3;
  reg_tree->min_leaf_samples = 5;


  fit(reg_tree, X, Y, n, m);

  float *predictions = predict(reg_tree, X, n, m);
```

Daarna declarer ik een *RegressionTree* op de heap, zodat ik het straaks kan fitten met *fit*. *fit* is het belangrijskte deel van dit programma en is verantwordelijk voor de splitting van X in een aantal bladjes.

*predict* geef dan een voorspelling voor elke observatie in X. Hoe goed is de gemideeld voorspelling? Dat kan door het mean squared error (mse) gechecked worden.

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

Ik doe een evaluatie door de mse van het tree model te vergelijken met die van een zogenoemd null model. In dit geval is het null model het gemideeld van alle Y waarden. In andere worden, een tree met een einige node.

```C
  float *feature_importances = compute_feature_importance(reg_tree, m);

  for (int i = 0; i < m; i++) {
    printf("Feat importance %d: %.3f\n", i, feature_importances[i]);
  }
```

In vergelijking met andere machine learning of ai methoden, zijn tree's vrij doorzichtig. Door de gain van elke split te toevoegen kan men een vertaling van welke variabel de meeste belangrijke zijn volgens het model.

Nu hebben we een vogelvlucht genomen over elke stap van de programma, kun jij keken naar hoe elke deel ervan is gebouwd.

## Sorteren
Het sorteren van de observaties is nodig om een gain voor elke trehshold te kunnen berekenen. Omdat ik het orde van een variabel will gebruiken om meerde arrays te sorteren, gebruik ik een functie dat geef de sorterend index terug. 

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

Ik gebruik het bubblesort algorithm omdat ik het gemakkelijk te herrineren vind. C heeft een eigen sorteren functie in de standard library, maar ik denk niet dat het of een alternatief functie de sorterend indices teruggeeft. 

Een andere detail is het gebruik van *memcpy*. Ik doe het zodat ik niet oef de data achter het *arr* dynamiek array te veranderen.

Wanneer ik de sorterend indices heb, moet een andere functie, *reorder*, het eingenlijk sorteren doen.

```C
float *reorder(float *arr, size_t *reorder_indices, size_t n) {
  float *reordered_arr = (float *)malloc(sizeof(float) * n);
  for (int i = 0; i < n; i++) {
    reordered_arr[i] = arr[reorder_indices[i]];
  }

  return reordered_arr;
}
```

## Een beetje rekenkunde

Door het programma zijn een aantal kleine functies die verantwordelijk zijn voor mathematiek operaties. Een heb je al gezien:

```C
float mse_compute(float *actuals, float *predictions, size_t n) {
  float sse = 0;
  for (int i = 0; i < n; i++) {
    sse += pow(actuals[i] - predictions[i], 2.0);
  }

  return sse/(float)n;
}
```

*mse_compute* is een van de eenvoudiger functie in de programma en kan voor een beginner plezant zijn om een beetje met C te spelen. Andere is het een heel gewoon functie en vraag dus geen uitleg.

Op hetzelfde niveau zit een nuttig *mean* functie:

```C
float mean(float *arr, size_t n) {
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum/(float)n;
}
```

Nu kan ik het hebben over de twee interessante rekenen functies: *compute\_gain* en *compute\_leaf\_value*. Laat me beginnen met *compute\_leaf\_value* beginnen. 

```C
float compute_leaf_value(float G_sum, float H_sum) {
  return -G_sum/H_sum;
}
```

Hier is het waarde van een bladjes berekend. *G_sum* is het sum van de gradients in het bladjes. Korte gezegd is hier een gradient het verschil tussen het gemiddeld van Y en een Y sample. Dus als je zeg maar twee Y samples heeft in een bladjes, zijn de gradients $\mathrm{mean}(Y) - Y\_1$ en $\mathrm{mean}(Y) - Y\_2$. Herriner je dat het hessian heeft hier altijd de waarde 1. Dan is geef *compute\_leaf\_value* gewoon het negatiev gemiddeld gradient van het bladjes.

Waarom neem ik de negatiev? Voorstel dat $Y\_1$ en $Y\_2$ zijn allebei groter dan $\mathrm{mean}(Y)$. Dan gan de gradients negatiev zijn, maar als jij het aantal door -1 vermenigvuldig, kreeg jij een bladjes waarde dat positiev is. En als jij aan het bladjes waarde het gemiddeld van Y toevoeg, krijg jij net een voorspelling dat zit tussen $Y\_1$ en $Y\_2$. 

De gradients zijn dus gebruikbaar niet enkel om node splits te vinden, maar ook om de voorspelling te maken. In die zin zijn de gradients hergebruikbaar.

In *compute\_gain* kan ik tonen hoe deze zijn gebruikt om een split te vinden. Dit gebeurd door de gains van alle mogelijk splits per feature of variabel te vergelijken.

```C
float compute_gain(float gradient_left, float gradient_right, float hessian_left, float hessian_right) {
  float left_side = pow(gradient_left, 2.0) / hessian_left + pow(gradient_right, 2.0) / hessian_right;
  float right_side = pow(gradient_left + gradient_right, 2.0) / (hessian_left + hessian_right);

  return left_side - right_side;
}
```

Ik ga met de *right\_size* van formule beginnen. Die geeft het gain als je het node niet split, maar wanneer jij het als bladjes zet. Het is het gain als je de tree niet laat groien.

De left_side van de formule keek naar het informatie die gegenererd is door de split. Als je nadenk over het formule, zie je dat het een vrij gangbare form heeft: $x^2 + y^2 - (x + y)^2, wat enkel positiev kan zijn als $x$ het minteken van $y$ heeft. Dus het doel van de regression tree is het variatie rond het gemiddeld van $Y$ zo goed mogelijk te uitleggen. Dit door het afhankelijken van $Y$ met $X$ te ontdekken.

## De correct fit zoeken

In vergelijken met boven discussies, is de fit functie eenvoudig. Het vind een gemiddeld van uitkomst Y, definiert de gradients en de hessians dynamic arrays, alloceer geheugen aan de *root* node en roep het recursive *\_split_node* aan. 

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

*\_split\_node* is een dicke functie met een groot aantal argumenten en meerdere voorwaardelijke branches. Ik ga het kort beshrijven en daarna de heel functie hier dumpen. De doel van die samenvatting is om structur te geven aan een aandachtig lezing van de *\_split\_node*.

Ik keek eerst of een split kan gemacht worden. Als de tree zijn *max\_depth* aangetroffen heeft, of als de split nodes een te klein aantal observaties zouden hebben, geef de functie de bladjes waarde terug met *compute\_leaf\_value* (zie de laatse *else* instructie). De fit van een branch van de tree is gedaan.

Waneer het tegenovergesteld gebeurd, dat is, wanneer een split gemacht kan worden, kruis de functie alle variabelen op zoek naar het variabel wiens split een best gain geef, en de *threshold* ervan. 

Veel code is dan verantwordelijk om de split te voorbereiden door de correcte gegevens aan de links en de rechts kinderen nodes te versturen. Om te weten welke waarde boven en welke waarde onder het threshold liggen, gebruik ik die MaskIndices struct. Als een waarde onder het threshold ligt, heb ik beslist om het naar de linkse node te sturen. Wat zou in twee python regels gebeuren, gebeurt soms met 30 linies van C.

Voor ik *\_split\_node* opnieuw aanroep, toewijz ik geheugen voor de linkse en rechtse kinderen nodes. Ik let ook op dat de depth van elke kinderen toenemt met een.

Een verschil met andere functies die gebruik zijn om recursie te leren, is dat elke roep aan *\_split\_node* iets teruggeef. Het edge voorwaarde is *should\_split*. 

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

Het eerste functie binnen *_split\_node* die angeroepen is is *should\_split*. Het ga keken naar een paar conditie om te weten of een split is toegelaten. Als ja, is er toch een verder check naar het *gain*. Het *gain* van de top gevonden split moet positiev zijn, andere geeft de regression tree zonder de nieuwe split een beter fit. 

Ik vind de *min\_leaf\_samples check interessant. $\mathrm{n_samples} / 2$ is het mininum groote van het grootse van de links en rechts kinderen nodes. Het $min\_leaf\_samples$ is dus het kleinste groote van het grooste kind dat een split toelate.

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

Het tweede angeroepen functie binnen *_split\_node* is *find\_best\_split*. Die zoekt naar een threshold op een array *arr* dat geeft het grooste gain terug. Die gain kreeg als argumenten de sum van de linkse en rechtse gradiente. De hessians zijn hier enkel gebruikt om de sums te normalizeren. 

Om ingewikkeld data masking te verminderen, gebruikt het vroeger getoond *arg\_sort* en *reorder* functies, samen met het gebruik van cumulative sums. Ik kan dus de linkse en rechtse sum eenvoudig kregen. Ik zou willen zegen dat het het efficienter manier om dingen te doen is, maar aangezien ik een bubblesort moet gebruiken, wet ik het niet. Naar mijn mening is het zeker gemakkelijk om te lezen.

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
```

Na ik de beste split threshold heb gevonden, moet ik wel masking gebruiken om de data ook te splitten en te sturen naar de relevante kinderen. Ik begin door de maximum groote van elke *indice* te deduceren, en zodra ik het werkelijke grootte van de indice arrays wet, aanroep ik realloc om de onnodige geheugen vrij te laten. 

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

Als jij tot hier bent geraken, gefeliciteerd, jij wet hoe je een regression tree kan fitten in C. Jij ga toch misschien willen blijven lesen. Ik ga het hebben over twee spannende tree traversal functie. Die zijn heel nuttig om wat sap te krijgen van de regression tree.

## Plezant tree traversals

Ik begin met mijn favoriet van de twee: het feature importance berekenen. Die geven het belang van elke variabel van *X*. Dit is gedaan door te vinden op welke variabel of feature elke split gedaan is, en die gain van die split toe te voegen aan een *feature_importances* array van grootse $1 \times m$ waar $m$ is het aantal variabelen.

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

Die recursive functie is gebruikt binnen de *compute\_feature\_importance* functie. Die doe een beetje boekhouding.

Het tweede tree traversal is gedaan om de voorspelling te kunnen doen. Voor elke observatie met waarde op alle variabelen van X, willen we een voorspelling kunnen doen. Dit gebeurd om de weg te vinden door de splits tot de bladjes met een voorspelling voor $x$ is getrokken. 

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

Kortom is dat gedaan door te keken, voor elke split tot een bladjes, of $x$ onder of over de threshold van die split feature is. Volgens gaan we naar links of rechtse, tot `c root->is_leaf` true is.

Nogmaals, doe ik een beetje boekhouding om voorspellingen te doen en te behouden voor elke $x$ binnen een matrix $X$:

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

## Mogelijke verder oefeningen

Er zijn een paar interessante oefeningen dat je kan doen om nog meer van regression trees.  

- Boven hebben we gebruik gemacht van de hele dataset om een split te vinden. Naarmate het aantal training observatie groter worde, worde dat minder interessant. Moderne tools zoals LightGBM en XGBoost kunnen gebruik maken van histogram binning om big data beter te handelen. In Python is het niet zo moieilijk om dit te doen met een paar numpy functies zoals *searchsorted* en *bincount*. Je kan het dus in Python proberen en dan het oplossing naar C vertalen.

Het idee is om de gradients en hessians per bin te summen om daarna het split te vinden door elke bin te zoeken in plaats van de hele dataset. Als je nog zogenoemd exact splits wil blijven vinden kan je ook een oplossing zoeken voor duplicate feature waarden. Dit kan ook efficientie geven, zeker als je met een kleine aantal waarden werk.

- Een andere oefeningen kan zijn om van de regression tree, random forest of gradient boosting machine maken. Tot een zekere mat is dat eigelijk eenvoudiger dan de oefening daarboven.

- Nog een interessant oefening zou zijn om een decision tree te maken in plaats van een regression tree. Nog ingewilkelder zou het zijn om meer dan twee discrete uitkomsten goed te voorspellen. 





