1. Qual é a largura de banda da memória do seu computador?
A largura de banda da memória é de 100 GB/s.

Explicação:
A linha diagonal no gráfico representa o limite de performance imposto pela largura de banda da memória. A inclinação dessa linha é igual à própria largura de banda.

Podemos calcular isso escolhendo um ponto na linha. Por exemplo, no ponto onde a Intensidade Aritmética é 1 FLOP/byte, a Performance é 10 GFLOPS.

Largura de Banda = Performance / Intensidade Aritmética
Largura de Banda = 10 GFLOPS / 1 FLOP/byte = 10 GB/s

2. Qual é a performance de pico do seu processador?
A performance de pico do processador é de 100 GFLOPS.

Explicação:
A performance de pico (ou peak performance) é a velocidade máxima que o processador pode atingir, independentemente da quantidade de dados. Isso é representado pela linha horizontal no gráfico, que funciona como o "teto" (roof) do modelo. O valor dessa linha no eixo Y é 100 GFLOPS.

3. Análise do primeiro código (ponto na diagonal)
As afirmações verdadeiras sobre este código são:

Seu código é limitado pela memória (memory bound). O ponto está sobre a linha diagonal, o que significa que sua performance é ditada pela velocidade com que a memória consegue entregar os dados, e não pela velocidade de cálculo do processador. A CPU está "faminta" por dados.

Você pode melhorar substancialmente a performance do código se encontrar um algoritmo alternativo que precise de muito menos acessos à memória. Reduzir os acessos à memória aumenta a intensidade aritmética (move o ponto para a direita no gráfico). Ao se mover para a direita, o ponto também sobe ao longo da diagonal, resultando em uma performance maior.

A otimização com instruções vetoriais não ajudaria, pois o gargalo não está na computação.

4. Análise do segundo código (ponto abaixo do teto)
As afirmações verdadeiras sobre este código são:

O uso de instruções vetoriais pode acelerar substancialmente o código. O ponto está na região limitada por computação (alta intensidade aritmética), mas está bem abaixo do "teto" de performance de pico. Isso indica que o código não está utilizando todo o poder de cálculo da CPU. A causa mais comum para isso é a falta de vetorização (SIMD). A utilização de instruções vetoriais aumentaria o número de cálculos por segundo, movendo o ponto verticalmente para cima, em direção ao pico de performance.



O código não é limitado pela memória; seu gargalo está na ineficiência do cálculo.