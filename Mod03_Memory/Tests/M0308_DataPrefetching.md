1. Por que a varredura linear é mais eficiente que a aleatória?
A resposta correta é:

A varredura linear é mais eficiente que a aleatória sempre que o conjunto de dados excede o tamanho do cache L1. Graças ao padrão previsível de acesso aos dados, novos dados podem ser pré-carregados (prefetched) enquanto a CPU trabalha nos dados atuais.

Explicação:
Quando você acessa a memória de forma sequencial (endereço i, i+1, i+2...), a CPU detecta esse padrão simples e previsível. Um componente de hardware chamado prefetcher se antecipa e busca os próximos blocos de memória (as próximas cache lines) da RAM (ou de um cache mais lento) para o cache L1 antes que a CPU precise deles. Isso efetivamente "esconde" a latência da memória, pois os dados já estão esperando no cache rápido quando são solicitados. Esse benefício é mais notável quando os dados são grandes demais para caberem no cache, forçando o sistema a buscar dados da memória principal constantemente. 🧠

2. Por que o acesso com passo (strided access) também é eficiente?
A resposta correta é:

Os algoritmos de prefetch de cache são geralmente capazes de detectar um padrão de acesso com passo e pré-carregar os dados de acordo.

Explicação:
Os prefetchers de hardware em CPUs modernas são mais sofisticados do que apenas detectar acessos sequenciais. Eles são projetados para reconhecer outros padrões, incluindo o acesso com um passo constante (por exemplo, acessar os endereços i, i+8, i+16, i+24...). Ao identificar esse padrão, o prefetcher começa a buscar os blocos de memória que contêm os próximos elementos da sequência, mesmo que não sejam adjacentes, mantendo a eficiência ao esconder a latência da memória.

3. Por que o acesso com passo de 64 bytes é menos eficiente que o linear?
A resposta correta é:

A CPU usa apenas um elemento por cache line. Sua operação é muito rápida e não deixa tempo suficiente para pré-carregar a próxima cache line.

Explicação:
Uma cache line (a unidade de transferência entre a RAM e o cache) tem tipicamente 64 bytes. Se o seu passo também é de 64 bytes, ocorre o seguinte:

O prefetcher carrega uma cache line de 64 bytes para o cache.

A CPU usa apenas um elemento (um double de 8 bytes) dessa linha. Os outros 56 bytes são "desperdiçados" para aquele acesso.

Imediatamente, a CPU precisa de um dado que está na próxima cache line.

Nesse cenário, a CPU consome os dados tão rapidamente (um por linha) que o prefetcher não consegue buscar a próxima linha a tempo, forçando a CPU a esperar. Essa espera é a latência do cache L2 que não pôde ser escondida.

4. Por que a performance cai ainda mais com um passo de 128 bytes?
Os efeitos que contribuem para essa desaceleração são:

O algoritmo de prefetch não consegue ir além dos limites da página. Com um passo maior, os limites de página são atingidos com mais frequência.

Com um passo maior, você consome páginas de memória a uma taxa mais alta. Isso leva a custosas falhas de cache TLB (TLB cache misses).

Explicação:
A memória não é apenas dividida em cache lines, mas também em páginas de memória (geralmente de 4 kB). A tradução de endereços virtuais para físicos para essas páginas é armazenada em um cache especial e muito rápido chamado TLB (Translation Lookaside Buffer).

Com passos muito grandes como 128 bytes, você cruza os limites dessas páginas de memória com muito mais frequência. Cada vez que você acessa uma nova página, o sistema precisa de sua tradução de endereço. Se essa tradução não estiver no TLB (o que se torna comum, pois o TLB é pequeno e você está "pulando" por muitas páginas), ocorre um TLB miss. Resolver um TLB miss é uma operação muito lenta que paralisa a CPU e degrada drasticamente a performance, explicando por que o tempo de acesso mais do que dobra.