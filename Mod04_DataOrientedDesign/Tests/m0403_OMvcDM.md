1. Por que a abordagem de programação orientada a objetos não é adequada para a otimização orientada a dados?
A razão fundamental é:

Porque uma das ideias centrais da programação orientada a objetos é esconder o layout real dos dados para suportar a abstração.

Explicação:
A Programação Orientada a Objetos (OOP) se baseia em princípios como encapsulamento e abstração. A meta é permitir que o programador use um objeto através de sua interface sem precisar saber como seus dados estão organizados internamente na memória.

Isso entra em conflito direto com a Otimização Orientada a Dados (DOD), cujo objetivo principal é exatamente o oposto: organizar os dados na memória de forma explícita e cuidadosa para que o hardware (cache, unidades vetoriais) possa processá-los da maneira mais eficiente possível. Para fazer isso, o programador precisa ter controle total sobre o layout da memória, muitas vezes quebrando abstrações clássicas (como usar uma Estrutura de Arrays em vez de um Array de Estruturas) para obter performance.

2. Como poderíamos substituir o polimorfismo virtual?
As alternativas comuns para substituir o polimorfismo virtual (palavra-chave virtual) são:

Com membros de enum class e instruções switch. Este é um padrão comum onde você adiciona um membro de tipo (enum) à sua estrutura base. Em vez de uma chamada de função virtual, uma função livre usa uma instrução switch nesse tipo para despachar a chamada para a implementação correta. Isso remove a indireção da vtable e pode ser mais facilmente otimizado pelo compilador.

Com variants. A classe std::variant da biblioteca padrão do C++ é uma união (union) segura que pode armazenar um de um conjunto fixo de tipos. Usando std::visit, você pode aplicar uma operação específica de tipo de forma segura e eficiente, sem alocação de ponteiros na heap ou vtables.

Usando RTTI (Run-Time-Type Information). Mecanismos como dynamic_cast e typeid permitem que você verifique o tipo dinâmico de um objeto em tempo de execução e execute o código apropriado. Embora funcional, essa abordagem é frequentemente considerada menos performática e elegante do que as alternativas com switch ou variant