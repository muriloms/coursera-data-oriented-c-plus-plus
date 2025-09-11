1. Quais problemas são abordados e potencialmente resolvidos com um esquema de endereçamento de memória virtual?
As questões que a memória virtual resolve são:

Fragmentação de memória: A memória virtual permite que um processo enxergue um bloco grande e contínuo de memória, mesmo que a memória física (RAM) esteja fragmentada em pequenos pedaços não adjacentes. O sistema operacional mapeia as páginas virtuais contíguas para quadros físicos espalhados, escondendo a complexidade do processo. 🧠

Segurança e estabilidade: Cada processo opera em seu próprio espaço de endereçamento virtual isolado. Isso impede que um processo acesse ou modifique diretamente a memória de outro, garantindo a segurança e evitando que uma falha em um programa afete os demais. 🛡️

A opção sobre correção automática de erros na memória física não é uma função da memória virtual.

2. Por que a primeira varredura de memória é mais lenta que as seguintes?
A explicação correta é:

A primeira varredura de memória gera falhas de página (page faults), pois os quadros de memória física ainda não foram atribuídos às suas páginas virtuais.

Quando você aloca a memória, o sistema operacional reserva o espaço de endereço virtual, mas não necessariamente aloca a memória física (RAM) imediatamente. Na primeira vez que seu programa tenta acessar uma porção dessa memória (uma "página"), o hardware detecta que não há memória física correspondente. Isso gera uma falha de página. O sistema operacional então interrompe seu processo, encontra um quadro de RAM livre, carrega os dados necessários (se houver) e mapeia essa RAM para a sua página virtual. Só então seu programa pode continuar.

Nas varreduras seguintes, esse mapeamento já foi feito e os dados estão na RAM (e possivelmente no cache da CPU), tornando o acesso muito mais rápido. ⚡

3. Quais mecanismos permitem que um processo solicite com sucesso um bloco de memória do tamanho da RAM, mesmo que ela já esteja parcialmente em uso?
Os mecanismos que tornam isso possível são:

Embora o SO forneça um ponteiro ao seu processo, ele não precisa garantir sua solicitação com memória física imediatamente. Ele pode procurar por páginas de memória física disponíveis quando você realmente as usar. Este mecanismo é chamado de lazy allocation ou alocação sob demanda. O SO "promete" a memória, mas só a entrega quando ela é de fato acessada, gerando uma page fault.

O SO pode mover a memória de outro processo para um dispositivo de armazenamento permanente (swap). Se a RAM estiver cheia quando seu processo precisar de uma página física, o SO pode pegar uma página menos usada de outro processo, salvá-la no disco (em uma área chamada swap space ou arquivo de paginação) e liberar aquele quadro de RAM para o seu processo usar. 💾

A opção de reverter alocações anteriores e retirar memória de outros processos de forma arbitrária não é um mecanismo padrão para essa situação.