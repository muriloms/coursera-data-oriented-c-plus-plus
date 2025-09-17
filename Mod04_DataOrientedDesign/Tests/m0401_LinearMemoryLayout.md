1. Por que o método segmentSpeed() pode ser lento?
A razão pela qual o método pode ser lento é:

Não há garantia de que os points sejam consecutivos na memória.

Explicação:
A estrutura Trajectory armazena um vector<Point*>, que é um vetor de ponteiros para objetos Point. Isso significa que os próprios objetos Point (com suas coordenadas x, y, t) estão alocados em diferentes locais na memória heap.

Quando o loop acessa points[i] e depois points[i+1], ele está acessando dois ponteiros que podem apontar para regiões de memória completamente distintas e distantes uma da outra. Esse padrão de acesso aleatório é péssimo para a performance porque:

Causa Cache Misses: A CPU carrega os dados de points[i] para o cache, mas os dados de points[i+1] não estão lá, forçando uma busca lenta na RAM.

Anula o Prefetcher: O hardware da CPU não consegue prever o próximo endereço de memória a ser acessado e, portanto, não pode pré-carregar os dados, que é uma otimização crucial para acessos lineares.

Esse problema de dados espalhados é um dos principais gargalos de performance em computação de alto desempenho.

2. Qual afirmação é verdadeira sobre SegmentA e SegmentB?
A afirmação verdadeira é:

Ambas as estruturas armazenam a mesma informação.

Explicação:
Logicamente, tanto SegmentA quanto SegmentB representam o mesmo conceito: um segmento de linha definido por um ponto de origem (from) e um ponto de destino (to). A diferença está na implementação e no layout da memória.

Vamos analisar por que as outras opções estão incorretas:

"ambas as estruturas têm o mesmo footprint de memória." Falso. SegmentA armazena dois objetos Point (2 * 3 * 8 bytes = 48 bytes). SegmentB armazena dois ponteiros (2 * 8 bytes = 16 bytes em um sistema de 64 bits). SegmentA é muito maior.

"struct SegmentB nunca será contígua na memória." Falso. A struct SegmentB em si (os dois ponteiros) é perfeitamente contígua. A afirmação provavelmente se refere aos dados Point que ela aponta, mas mesmo esses poderiam ser alocados de forma contígua (new Point[2]), tornando a palavra "nunca" incorreta.