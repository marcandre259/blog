---
layout: math
title: Regression tree algorithm van niets in C
date: 2025-10-09
---

## Motivatie
In deze blogpost beschrijf ik een basaal regression tree algorithm in C. Regression en decision trees zijn vandaag overal te vinden in het machine learning landschap. Nog in 2025 en ondanks het optreden van pre-trained one-shot transformer models, zijn gradient boosting machines (GBM) vaak de beste methoden om goede voorspellingen te maken (zie bvb. W. Rizkallah, Journal of Big Data, 2025). Ook in de industrie kom ik vaak algorithmen tegen zoals LightGBM die variaties zijn op de beroemde GBM. 

Van alle onderdelen die deel uitmaken van GBM's en hun verschillende implementaties, is de regression of decision tree het belangrijkste. Bovenop andere innovaties en componenten zoals loss functies, feature binning, gradient descent, one-side sampling, zitten die trees aan de basis van GBM's en gerelateerde algorithmen zoals de Random Forest. 

Het verschil tussen regression en decision zit in de uitkomst die de analist probeert te voorspellen. Is de uitkomst discreet: ja of nee, met twee of meer categorieën, dan heb je met een decision tree te maken. Is de uitkomst continu, huisprijzen bijvoorbeeld, dan heb je met een regression tree te maken. 

In wezen gebruiken beide trees dezelfde fit strategie. In een gedefinieerd aantal stappen, delen zij een data sample in een aantal bladjes (leaves) zodat het verschil tussen elke bladjeswaarden en zijn gerelateerde uitkomsten het kleinste  is. Nu, als je gewoon het verschil tussen uitkomsten en sample bladjes wilt verminderen, zou je het beste vinden om een bladje per observatie te definiëren. Maar dan heeft het tree model geen generaliserend vermogen. Dus moeten er concessies gedaan worden in de richting van generalisatie. Dit gebeurt door het zetten van parameters zoals het minimum aantal data per bladje, de maximale diepte van de tree of de minimale gain die toegestaan is. 

## Structuur
De bedoeling achter deze post is dat jij zelf kunt leren hoe regression trees werken door het algoritme zelf in C te schrijven. Dus ga ik de nodige functies geven zonder ze samen te zetten. Volgens mij is dat een plezante manier om iets te leren. Ik ben zelf zeker geen C expert, dus let op want er gaan memory leaks in de code zijn. Ik geef helemaal geen aandacht aan de leaks. Dit is zeker geen code dat klaar voor productie is.

Ik ga proberen de intuïtie en de logica achter elke code snippet door te geven. Tot een zekere hoogte.

De focus blijft op de implementatie en niet op de intuïtie, dus raad ik aan die die meer intuïtie nodig hebben om het te vinden op [scikit-learn](https://scikit-learn.org/stable/auto_examples/tree/plot_tree_regression.html#sphx-glr-auto-examples-tree-plot-tree-regression-py).

## Nuttige concepten
De implementatie gebruikt een mooi aantal concepten die komen van programmatie en statistiek domeinen.

Om de beste splits tussen bladjes te vinden moet de data gesorteerd worden volgens de orde van grootte van elke variabele. Het is dus nodig om een sorting algorithm te gebruiken. 

Om te kunnen beslissen of een split bevorderlijk is, gebruiken we zogenoemde gradients, hessians en een gain formule. In dit geval, zijn de gradients min of meer gecentreerde uitkomsten, en de hessians zijn unit vectors.

Recursie gaat vaak gebruikt worden. De tree zelf is gebouwd door een recursief algorithm. Om de gefitte trees te kunnen gebruiken om voorspellingen te doen moet je ze ook met recursie doorkruisen. Tree traversal is ook gebruikt om de feature importance te berekenen. Men kan zeggen dat een regression tree een weg geeft aan elk observatie (of instancie in machine learning taal...), en dat die weg gevolgd moet worden door recursie. Dus recursie gaat hierbeneden een heel belangrijke rol spelen.

Om het algorithm te kunnen testen, gebruik ik ook de simpele random number generator (RNG) van de C standard library. Data simulatie is dus ook een aanwezig concept.

Daarna zijn er een aantal dingen die geassocieerd zijn met het gebruik van C. Namelijk: pointers en memory addresses, stack en heap, struct's en data types. Ik heb enkel een praktisch begrip van die concepten en ga ze vandaar zonder diepte proberen uit te leggen. Het belangrijkste daar is te weten dat een dynamische array in C een pointer is naar het eerste memory address van de array's data. De analist moet dus altijd goed bijhouden hoe lang die array is. Anders krijg jij rommel. 

Ook belangrijk voor recursieve functies is het verschil tussen data die zit op de stack en data die zit op de heap. Data op de stack wordt verwijderd zodra het buiten de huidige scope van het programma zit. Data op de heap is het tegenovergestelde. Het wordt bijgehouden tot het vrijgelaten wordt door de analist. Dus heap data kan veranderd worden in een functie en die veranderde data kan binnen de scope van een tweede functie gebruikt worden. Dat is vaak nodig met recursie.

## Ingrediënten

Het is goed om te beginnen met het kleine aantal ingrediënten die gebruikt zullen worden. Het eerste houdt info bij over een mogelijke split. Het is zo gedefinieerd:

```C
typedef struct SplitInfo {
  float gain; 
  float threshold; 
} SplitInfo;
```

Die SplitInfo struct moet gegevens over de beste split van elke feature behouden. Elke split heeft een threshold, waardoor observaties wiens waarde op die feature kleiner is dan de threshold naar de zogenoemde left children node gegeven worden, en de rest naar de right children node. 

De gain wordt behouden om twee redenen. Ten eerste om de beste split van verschillende features te kunnen vergelijken. Ten tweede kan elke split gain gebruikt worden om feature importance te berekenen.

De tree zelf is een verzameling van gerelateerde nodes. Elke node heeft een diepte. Op diepte nul vindt men de root node die de hele dataset bevat. Op diepte één is deze root node gedeeld in twee children nodes. Elk deel kreeg zijn eigen observaties. Dan worden elk van die twee nodes op diepte één nogmaals in twee gedeeld enzovoort. Op de laatste diepte zitten de bladjes. Die nodes geven voorspellingen op basis van de voorafgaande splitsingen. De regression tree is eigenlijk een binary tree.

Elke node is dus zo gedefinieerd:

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

De left en right Node's zijn pointers naar children nodes. Omdat elke Node zijn eigen kinderen omvat kan men zeggen dat het een recursief object is. Daarom gebruik ik pointers voor de left en right nodes. Anders zou een Node object een oneindig aantal kinderen en klein-kinderen nodes hebben.

Een Node heeft ook een feature_id en een threshold, om te behouden op welke feature en op welke waarde de data gesplitst was. Om te weten of de tree groot genoeg is, behoud ik ook de depth van de node. 

Als een node een bladje is, krijgt het een waarde van *true* op *is_leaf*. Anders krijgt de node daar een waarde van *false*. Bladjes krijgen ook een *value*, maar geen feature_id of threshold aangezien zij geen children nodes hebben.

Eindelijk definieer ik de *RegressionTree* struct:

```C
typedef struct RegressionTree {
  int max_depth;
  int min_leaf_samples;
  float constant;
  Node *root;

} RegressionTree;
```

De tree bevat de root node, twee complexiteit beperkingen en een constante. Vanaf de root node kan je de hele tree doorkruisen. De complexiteit beperkingen zijn gebruikt om iets toe te geven aan generaliteit. In het kort omdat ik voorspellingen op nieuwe data wil kunnen doen met het model. 

De constante is het gemiddelde van de uitkomst waarden. Door de uitkomsten van de constante af te trekken, krijg ik de gradients. Dit mag op een omweg lijken maar gaat eigenlijk helpen met de berekening van de gains. 

Een laatste struct die ik gebruik is de *MaskIndices*:

```C
typedef struct MaskIndices {
  int *left_indices;
  size_t left_n;
  int *right_indices;
  size_t right_n;
} MaskIndices;
```

Zodra jij een split threshold vindt, ga je willen weten welke van die observaties moet naar de linkse en welke naar de rechtse children nodes. Deze gegevens worden door de linkse en de rechtse indices behouden. Met C is het ingewikkeld om de hoeveelheid onderdelen van een dynamische array te berekenen, dus neem ik ook *left_n* en *right_n* mee.

## Functies

### Main functie
Oké, eindelijk treffen wij de inhoud. Laat me beginnen met het einde van dit project: de *main* functie. Die omvat alle belangrijke stappen, van de declaratie van een gesimuleerde dataset en een regression tree model tot de ontdekking van de feature importances.

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

Om te beginnen creëer ik een dataset met drie features X en een uitkomst Y. De features zitten binnen de dubbele pointer X. Van die dubbele pointer zijn de drie pointer columns toegankelijk. We gaan veel met de pointers spelen aangezien ze nodig zijn om dynamische arrays zoals X en Y te bouwen in C.

Om waarde te geven aan X neem ik samples van een uniforme distributie. *rand* geeft een waarde tussen 0 en *RAND_MAX* terug. Dus door te delen met RAND_MAX krijg jij een waarde tussen 0 en 1. Goed genoeg om te testen. 

Y is compleet afhankelijk van de eerste en tweede kolom van X. Dus, als ik kijk naar de feature importances, zou ik een nul importance waarde zien voor de derde kolom van X.

```C
  RegressionTree *reg_tree = malloc(sizeof(RegressionTree));

  reg_tree->max_depth = 3;
  reg_tree->min_leaf_samples = 5;


  fit(reg_tree, X, Y, n, m);

  float *predictions = predict(reg_tree, X, n, m);
```

Daarna declareer ik een *RegressionTree* op de heap, zodat ik het straks kan fitten met *fit*. *fit* is het belangrijkste deel van dit programma en is verantwoordelijk voor de splitsing van X in een aantal bladjes.

*predict* geeft dan een voorspelling voor elke observatie in X. Hoe goed is de gemiddelde voorspelling? Dat kan door de mean squared error (mse) gecheckt worden.

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

Ik doe een evaluatie door de mse van het tree model te vergelijken met die van een zogenoemd null model. In dit geval is het null model het gemiddelde van alle Y waarden. In andere woorden, een tree met een enige node.

```C
  float *feature_importances = compute_feature_importance(reg_tree, m);

  for (int i = 0; i < m; i++) {
    printf("Feat importance %d: %.3f\n", i, feature_importances[i]);
  }
```

In vergelijking met andere machine learning of ai methoden, zijn trees vrij doorzichtig. Door de gain van elke split te sommeren kan men een vertaling krijgen van welke variabelen de meeste belangrijke zijn volgens het model.

Nu we een vogelvlucht genomen hebben over elke stap van het programma, kun jij kijken naar hoe elk deel ervan is gebouwd.

## Sorteren
Het sorteren van de observaties is nodig om een gain voor elke threshold te kunnen berekenen. Omdat ik de orde van een variabele wil gebruiken om meerdere arrays te sorteren, gebruik ik een functie die de sorterende index teruggeeft in plaats van de gesorteerd input array *arr*. 

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

Om te sorten gebruik ik het bubblesort algorithm omdat ik het gemakkelijk te herinneren vind. C heeft een eigen sorteren functie in de standard library, maar ik denk niet dat het of een alternatieve functie de sorterende indices teruggeeft. 

Een ander detail is het gebruik van *memcpy*. Ik doe het zodat ik niet per ongeluk de data achter het *arr* dynamische array verander.

Wanneer ik de sorterende indices heb, moet een andere functie, *reorder*, het eigenlijke sorteren doen.

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

Door het programma zijn er een aantal kleine functies die verantwoordelijk zijn voor mathematische operaties. Eén heb je al gezien:

```C
float mse_compute(float *actuals, float *predictions, size_t n) {
  float sse = 0;
  for (int i = 0; i < n; i++) {
    sse += pow(actuals[i] - predictions[i], 2.0);
  }

  return sse/(float)n;
}
```

*mse_compute* is een van de eenvoudigere functies in het programma en kan voor een beginner plezant zijn om een beetje met C te spelen. Anders is het een heel gewone functie en vraagt dus geen uitleg.

Op hetzelfde niveau zit een nuttige *mean* functie:

```C
float mean(float *arr, size_t n) {
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum/(float)n;
}
```

Nu kan ik het hebben over de twee interessante rekenen functies: *compute\_gain* en *compute\_leaf\_value*. Laat me met *compute\_leaf\_value* beginnen. 

```C
float compute_leaf_value(float G_sum, float H_sum) {
  return -G_sum/H_sum;
}
```

Hier wordt de waarde van een bladje berekend. *G_sum* is de som van de gradients in het bladje. Kort gezegd is hier een gradient het verschil tussen het gemiddelde van Y en een Y sample. Dus als je zeg maar twee Y samples hebt in een bladje, zijn de gradients $\mathrm{mean}(Y) - Y\_1$ en $\mathrm{mean}(Y) - Y\_2$. Herinner je dat de hessian hier altijd de waarde 1 heeft. Dan geeft *compute\_leaf\_value* gewoon het negatieve gemiddelde gradient van het bladje.

Waarom neem ik het negatieve? Veronderstel dat $Y\_1$ en $Y\_2$ zijn allebei groter dan $\mathrm{mean}(Y)$. Dan gaan de gradients negatief zijn, maar als jij het getal door -1 vermenigvuldigt, krijg jij een bladjeswaarde die positief is. En als jij aan de bladjeswaarde het gemiddelde van Y toevoegt, krijg jij net een voorspelling die zit tussen $Y\_1$ en $Y\_2$. 

De gradients zijn dus bruikbaar niet enkel om node splits te vinden, maar ook om de voorspelling te maken. In die zin zijn de gradients herbruikbaar.

In *compute\_gain* kan ik tonen hoe deze gebruikt worden om een split te vinden. Dit gebeurt door de gains van alle mogelijke splits per feature of variabele te vergelijken.

```C
float compute_gain(float gradient_left, float gradient_right, float hessian_left, float hessian_right) {
  float left_side = pow(gradient_left, 2.0) / hessian_left + pow(gradient_right, 2.0) / hessian_right;
  float right_side = pow(gradient_left + gradient_right, 2.0) / (hessian_left + hessian_right);

  return left_side - right_side;
}
```

Ik ga met de *right_side* van de formule beginnen. Die geeft de gain als je de node niet splitst, maar daarentegen als je het als bladje zet. In andere worden is het de gain als je de tree niet laat groeien.

De left_side van de formule kijkt naar de informatie die gegenereerd wordt door de split. Als je nadenkt over de formule, zie je dat het een vrij gangbare vorm heeft: $x^2 + y^2 - (x + y)^2$, wat enkel positief kan zijn als $x$ het minteken van $y$ heeft. Dus het doel van de regression tree is de variatie rond het gemiddelde van $Y$ zo goed mogelijk te verklaren. Dit door de afhankelijkheid van $Y$ met $X$ te ontdekken.

## De correcte fit zoeken

De fit functie is relatief eenvoudig. Het vindt het gemiddelde van uitkomst Y, definieert de gradients en de hessians dynamische arrays, alloceert geheugen aan de *root* node en roept het recursieve *\_split_node* aan. 

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

*\_split\_node* is een dikke functie met een groot aantal argumenten en meerdere voorwaardelijke branches. Ik ga het kort beschrijven en daarna de hele functie hier dumpen. Het doel van die samenvatting is om structuur te geven aan een aandachtige lezing van *\_split\_node*.

Ik kijk eerst of een split gemaakt kan worden. Als de tree zijn *max\_depth* aangetroffen heeft, of als de gesplitste nodes een te klein aantal observaties zouden hebben, geeft de functie de bladjeswaarde terug met *compute\_leaf\_value* (zie de laatste *else* instructie). De fit van een branch van de tree is gedaan.

Wanneer het tegenovergestelde gebeurt, dat is, wanneer een split gemaakt kan worden, kruist de functie alle variabelen door op zoek naar de variabele wiens split een beste gain geeft, en de *threshold* ervan. 

Veel code is dan verantwoordelijk om de split voor te bereiden door de correcte gegevens aan de linkse en de rechtse children nodes te versturen. Om te weten welke waarde boven en welke waarde onder de threshold liggen, gebruik ik die MaskIndices struct. Als een waarde onder de threshold ligt, heb ik beslist om het naar de linkse node te sturen. Wat in twee Python regels zou gebeuren, gebeurt soms met 30 lijnen van C.

Voordat ik *\_split\_node* opnieuw aanroep, wijs ik geheugen toe voor de linkse en rechtse children nodes. Ik let ook op dat de depth van elk kind toeneemt met één.

Een verschil met andere functies die gebruikt worden om recursie te leren, is dat elke roep aan *\_split\_node* iets teruggeeft. De edge voorwaarde van de recursie is *should\_split*. 

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

De eerste functie binnen *_split\_node* die aangeroepen wordt is *should\_split*. Het gaat kijken naar een paar condities om te weten of een split is toegestaan. Als ja, is er toch een verdere check naar de *gain*. De *gain* van de top gevonden split moet positief zijn, anders geeft de regression tree zonder de nieuwe split een betere fit. 

Ik vind de *min\_leaf\_samples* check interessant. $\frac{\text{n\_samples}}{2}$ is de minimale grootte van het grootste van de linkse en rechtse children nodes. Het *min\_leaf\_samples* is dus de kleinste aantal observaties van het grootste kind dat een split toestaat.

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

De tweede aangeroepen functie binnen *_split\_node* is *find\_best\_split*. Die zoekt naar een threshold op een array *arr* die de grootste gain teruggeeft. Die gain krijgt als argumenten de sommen van de linkse en rechtse gradiënten. De hessians zijn hier enkel gebruikt om de sommen te normaliseren. 

Om ingewikkelde data masking te verminderen, gebruik ik de eerder getoonde *arg\_sort* en *reorder* functies, samen met het gebruik van cumulative sommen. Ik kan dus de linkse en rechtse som eenvoudig krijgen. Ik zou willen zeggen dat het de efficiëntere manier om dingen te doen is, maar aangezien ik een bubblesort moet gebruiken, weet ik het niet. Naar mijn mening is het zeker gemakkelijk om te lezen.

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

Nadat ik de beste split threshold heb gevonden, moet ik wel masking gebruiken om de data ook te splitsen en te sturen naar de relevante kinderen. Ik begin door de maximale grootte van elke *indice* te deduceren, en zodra ik de werkelijke grootte van de indice arrays weet, roep ik realloc aan om het onnodige geheugen vrij te laten. 

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

Als jij tot hier bent geraakt, gefeliciteerd, jij weet hoe je een regression tree kan fitten in C. Jij gaat toch misschien willen blijven lezen. Ik ga het hebben over twee spannende tree traversal functies. Die zijn heel nuttig om wat sap te krijgen van de regression tree.

## Plezante tree traversals

Ik begin met mijn favoriet van de twee: het berekening van de feature importances. Deze geven het belang van elke variabele van *X*. Dit wordt gedaan door te vinden op welke variabele of feature elke split gedaan is, en de gain van die split toe te voegen aan een *feature_importances* array van grootte $1 \times m$ waar $m$ het aantal variabelen is.

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

Die recursieve functie wordt gebruikt binnen de *compute\_feature\_importance* functie. Die doet een beetje boekhouding.

De tweede tree traversal wordt gedaan om de voorspelling te kunnen doen. Voor elke observatie met waarde op alle variabelen van X, willen we een voorspelling kunnen doen. Dit gebeurt door de weg te vinden door de splits tot het bladje met een voorspelling voor $x$ is getroffen. 

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

Kortom wordt dat gedaan door te kijken, voor elke split tot een bladje, of $x$ onder of over de threshold van die split feature is. Volgens gaan we naar links of rechts, tot `root->is_leaf` true is.

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

## Mogelijke verdere oefeningen

Er zijn een paar interessante oefeningen die je kunt doen om nog meer te leren van regression trees.  

- Hierboven hebben we gebruik gemaakt van de hele dataset om een split te vinden. Naarmate het aantal training observaties groter wordt, wordt dat minder interessant. Moderne tools zoals LightGBM en XGBoost kunnen gebruik maken van histogram binning om big data beter te handelen. In Python is het niet zo moeilijk om dit te doen met een paar numpy functies zoals *searchsorted* en *bincount*. Je kunt het dus in Python proberen en dan de oplossing naar C vertalen.

  Het idee is om de gradients en hessians per bin te sommeren om daarna de split te vinden door elke bin te zoeken in plaats van de hele dataset. Als je nog zogenoemde exacte splits wilt blijven vinden kun je ook een oplossing zoeken voor duplicate feature waarden. Dit kan ook efficiëntie geven, zeker als je met een klein aantal waarden werkt.

- Een andere oefening kan zijn om van de regression tree, een random forest of een gradient boosting machine te maken. Tot een zekere mate is dat eigenlijk eenvoudiger dan de oefening daarboven.

- Nog een interessante oefening zou zijn om een decision tree te maken in plaats van een regression tree. Nog ingewikkelder zou het zijn om meer dan twee discrete uitkomsten goed te voorspellen.
