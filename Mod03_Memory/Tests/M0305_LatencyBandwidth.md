1. Tempo de transferência para 1 kB de dados
Para um bloco de 1 kB, o tempo de transferência é de 110 ns.

Cálculo:

A fórmula é t = L + N / BW.

Converter as unidades: Primeiro, garantimos que todas as unidades são compatíveis (segundos e bytes).

L (Latência): 100 ns = 100 x 10⁻⁹ s

N (Tamanho): 1 kB = 10³ bytes

BW (Largura de Banda): 100 GB/s = 100 x 10⁹ bytes/s

Calcular o tempo da largura de banda (N / BW):

t_bw = (10³ bytes) / (100 x 10⁹ bytes/s) = 0,01 x 10⁻⁶ s = 10 x 10⁻⁹ s

Isso é igual a 10 ns.

Somar a latência:

t = L + t_bw = 100 ns + 10 ns = 110 ns

2. Tempo de transferência para 1 MB de dados
Para um bloco de 1 MB, o tempo de transferência é de 10.100 ns.

Cálculo:

A fórmula e os valores de L e BW são os mesmos. Apenas o tamanho N muda.

Converter as unidades:

L: 100 ns

N: 1 MB = 10⁶ bytes

BW: 100 GB/s = 100 x 10⁹ bytes/s

Calcular o tempo da largura de banda (N / BW):

t_bw = (10⁶ bytes) / (100 x 10⁹ bytes/s) = 0,01 x 10⁻³ s = 10.000 x 10⁻⁹ s

Isso é igual a 10.000 ns.

Somar a latência:

t = L + t_bw = 100 ns + 10.000 ns = 10.100 ns

📈 Observação: Note como para transferências maiores, o tempo gasto com a largura de banda se torna muito mais significativo do que a latência inicial.

3. Tempo de transferência para 1 MB de dados em 1000 blocos
Para 1 MB de dados divididos em 1000 blocos, o tempo total de transferência é de 110.000 ns.

Cálculo:

O ponto crucial aqui é que a latência L é paga para cada um dos 1000 blocos. A largura de banda, no entanto, é paga apenas sobre a quantidade total de dados transferidos.

Calcular a latência total:

L_total = k * L = 1000 * 100 ns = 100.000 ns

Usar o tempo de largura de banda da questão anterior:

A quantidade total de dados ainda é 1 MB, então o tempo de largura de banda é o mesmo calculado na Questão 2.

t_bw = 10.000 ns

Somar a latência total com o tempo de largura de banda:

t = L_total + t_bw = 100.000 ns + 10.000 ns = 110.000 ns

💥 Impacto: Este resultado demonstra o enorme custo de acessar dados de forma fragmentada na memória. A penalidade de latência múltipla (100.000 ns) dominou completamente o tempo total de transferência.