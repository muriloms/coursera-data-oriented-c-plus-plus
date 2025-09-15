1. Organização de memória de uma thread
As afirmações verdadeiras são:

Você deve colocar pequenas quantidades de dados (ex: floats individuais) na stack, pois os mecanismos de alocação e desalocação da stack são excepcionalmente rápidos. A alocação/desalocação na stack envolve apenas ajustar um ponteiro, sendo a forma mais rápida de gerenciar memória.

Quando você aloca um vector da biblioteca padrão, seus dados são colocados na heap. O objeto std::vector em si pode estar na stack, mas o array de dados que ele gerencia é alocado dinamicamente na heap, permitindo que ele cresça e diminua de tamanho.

2. Memória Virtual
As afirmações verdadeiras são:

A quantidade total de memória alocada por todas as threads pode exceder a quantidade de memória física disponível. Este é um recurso chave da memória virtual, que utiliza o disco (swap) para estender a RAM.

Com a memória virtual, pode ser possível que uma thread aloque uma memória contígua de um determinado tamanho, mesmo quando a memória está fragmentada e nenhum bloco físico único do tamanho solicitado está disponível. A memória virtual é contígua do ponto de vista do programa, mas o sistema operacional mapeia essas páginas virtuais para quadros físicos que podem estar espalhados pela RAM.

3. Unidade de Gerenciamento de Memória (MMU)
As afirmações verdadeiras são:

No pior caso, uma tradução do número da página para o número do quadro custa um acesso à RAM, para uma consulta na tabela de páginas. Se a tradução não estiver no TLB (cache), a MMU deve consultar a tabela de páginas, que reside na RAM.

O TLB, ou "Translation Lookaside Buffer", frequentemente evita "page walks" caros ao armazenar em cache a tradução de um número de página para um número de quadro. O TLB é um cache de alta velocidade que armazena as traduções usadas recentemente, acelerando o processo.

A MMU implementa os mecanismos de memória virtual. A MMU é o componente de hardware que realiza a tradução de endereços virtuais para físicos e impõe a proteção de memória.

4. Latência vs. Largura de Banda
A latência conta por metade do tempo total de transferência para um tamanho de dados de 2 MB.

Cálculo:
O tempo total de transferência (t) é Latência (L) + Tamanho (N) / Largura de Banda (BW). Queremos o ponto em que a latência é metade do tempo total, ou seja, L = t / 2, o que implica que a outra metade também é igual a L.
Portanto, L = N / BW.

Resolvendo para N:
N = L * BW
N = (10 ms) * (200 MB/s)
N = (10 * 10⁻³ s) * (200 MB/s) = 2 MB

5. Memória Cache
A afirmação correta é:

L1, L2 e L3 referem-se a diferentes níveis de cache, sendo L1 o mais rápido, mas com a menor capacidade. Isso descreve corretamente a hierarquia de cache, onde a velocidade e o custo diminuem enquanto o tamanho aumenta do L1 para o L3.

6. Unidades Vetoriais
As afirmações verdadeiras são:

Unidades vetoriais em uma CPU são projetadas especificamente para lidar com instruções únicas que operam em múltiplos pontos de dados simultaneamente. Esta é a definição de SIMD (Single Instruction, Multiple Data), que é a base da computação vetorial.

A afirmação sobre AVX-512 e 16 doubles está incorreta; AVX-512 processa 8 doubles (512 bits / 64 bits por double).

7. Análise de Plataforma de Hardware
As afirmações corretas são:

Se um código é limitado por computação (compute-bound) nesta plataforma, sua performance é limitada a no máximo 400 GFLOPS. Por definição, um código limitado por computação atinge seu teto na performance de pico do processador.

Se um código é limitado por memória (memory-bound) nesta plataforma, uma medida típica para melhorar a performance seria procurar um algoritmo alternativo com uma maior intensidade aritmética. Aumentar a intensidade aritmética (fazer mais cálculos por byte lido) é a principal maneira de otimizar um código limitado pela memória.

8. Intensidade Aritmética (AI) - Parte 1
A intensidade aritmética do algoritmo é 0.125.

Cálculo:
Intensidade Aritmética (AI) = (Total de FLOPS) / (Total de Bytes Lidos)

FLOPS: 1 computação aritmética = 1 FLOP.

Bytes: 1 número double = 8 bytes.

AI = 1 FLOP / 8 bytes = 0.125 FLOPS/byte

9. Performance de Pico Esperada - Parte 1
A performance de pico que você pode esperar é 100 GFLOPS.

Cálculo:
A performance alcançável é o mínimo entre a performance de pico do processador e a performance limitada pela largura de banda da memória.
Performance = min(Pico de Performance, AI * Largura de Banda)

Pico de Performance: 400 GFLOPS

Performance da Memória: AI * Largura de Banda = 0.125 FLOPS/byte * 800 GB/s = 100 GFLOPS

Performance = min(400 GFLOPS, 100 GFLOPS) = 100 GFLOPS. O código é limitado pela memória.

10. Performance de Pico Esperada - Parte 2
A performance de pico que você pode esperar é 400 GFLOPS.

Cálculo:
Primeiro, calculamos a nova intensidade aritmética.

FLOPS: 5 operações aritméticas = 5 FLOPS.

Bytes: 1 número double = 8 bytes.

AI = 5 FLOPS / 8 bytes = 0.625 FLOPS/byte

Agora, calculamos a performance alcançável:
Performance = min(Pico de Performance, AI * Largura de Banda)

Pico de Performance: 400 GFLOPS

Performance da Memória: AI * Largura de Banda = 0.625 FLOPS/byte * 800 GB/s = 500 GFLOPS

Performance = min(400 GFLOPS, 500 GFLOPS) = 400 GFLOPS. O código agora é limitado pela computação.