1. Como explicar a diferença de tempo de acesso entre 32 kB e 32 GB?
A explicação correta para essa grande diferença de performance é:

Este experimento simplesmente mostra a latência do cache L1 e a latência da RAM, que têm aproximadamente os valores indicados na questão.

Análise:

Acesso a 32 kB (1.5 ns): Um conjunto de dados de 32 kB é pequeno o suficiente para caber inteiramente no cache L1 da maioria dos processadores modernos. Mesmo com acessos em ordem aleatória, após os primeiros acessos ("aquecimento"), todo o conjunto de dados estará no cache. A partir daí, qualquer acesso aleatório dentro desses 32 kB será um acerto no L1 (cache hit), resultando em um tempo de acesso extremamente baixo, que é a latência do L1 (tipicamente em torno de 1-2 ns). ⚡️

Acesso a 32 GB (118 ns): Um conjunto de dados de 32 GB é imenso e não cabe em nenhum nível de cache da CPU (L1, L2 ou L3). Portanto, cada acesso aleatório tem uma probabilidade altíssima de ser uma falha de cache (cache miss) em todos os níveis, forçando o processador a buscar o dado diretamente na memória RAM. O tempo medido de 118 ns corresponde à latência da RAM, que é ordens de magnitude mais lenta que o cache. 🐢

2. Qual o tempo de acesso esperado para o conjunto de dados de 100.000 doubles?
O tempo de acesso esperado para cada elemento, após o aquecimento inicial, será de 4 ns.

Análise:

Calcular o tamanho total do conjunto de dados:

Um valor double (ponto flutuante de precisão dupla) ocupa 8 bytes.

Tamanho total = 100.000 valores * 8 bytes/valor = 800.000 bytes.

Converter para kilobytes (kB) para comparar com o cache:

800.000 bytes / 1.024 bytes/kB ≈ 781 kB.

Verificar em qual nível de cache o conjunto de dados se encaixa:

Cache L1: 128 kB. (781 kB não cabe aqui).

Cache L2: 1 MB (ou 1024 kB). (781 kB cabe confortavelmente aqui).

Cache L3: 8 MB.

Conclusão:
Como o conjunto de dados de 781 kB cabe inteiramente no cache L2 de 1 MB e os acessos são repetidos, o sistema irá carregar todos os dados para o cache L2. Após essa fase inicial, todos os acessos aleatórios subsequentes serão atendidos diretamente pelo L2. Portanto, o tempo de acesso medido será a latência do cache L2, que é de 4 ns.