Enunciado:
- Há um banheiro unissex com X boxes
- Y pessoas vão usar o banheiro
- Em intervalos de tempo aleatórios, uma pessoa (homem ou mulher, 50% de chance cada) entra na fila
- Se houver pelo menos um box livre quando a pessoa está na primeira posição da fila, ela imediatamente entra no banheiro, ocupando um box livre
- A pessoa usa o box por um tempo aleatório, depois imediatamente o desocupa
- Homens só podem entrar se não houver nenhum box ocupado por mulheres, e vice-versa

Na minha implementação, o programa imprime o estado atual da fila sempre que uma pessoa entra ou sai dela (início à esquerda, final à direita)
Ex: HMMHHMHHH

Da mesma forma, ele imprime o estado atual dos boxes sempre que uma pessoa sai ou entra em algum box.
Ex: |M| |M|M|

Ao longo do código, pode-se notar seções usando labels em conjunto com chaves. Isto é uma opção pessoal, pois eu gosto da utilidade que as chaves provêem delimitando escopos locais no meio de funções (relevante neste programa, pois permite a liberação de locks no meio de funções sem ter que se chamar unlock explicitamente), em conjunto com labels para adicionar legibilidade e organização. Poderia-se refatorar esses trechos para eles serem funções separadas, mas, sendo trechos com poucas linhas (por volta de 4 ou 5, em média), pra mim isso só iria adicionar boilerplate desnecessário, além de deixar a leitura do código mais chata, necessitando mais idas e voltas.

Exemplo de saída (TOTAL_PESSOAS = 10; NUM_BOXES = 3):
H

|H| | |
M
MM
MMM
MMMH
| | | |
MMH
|M| | |
MH
|M|M| |
H
|M|M|M|
HM
HMM
HMMM
|M| |M|
HMMMM
HMMMMM
Chegaram todas as pessoas!
| | |M|
| | | |
MMMMM
|H| | |
| | | |
MMMM
|M| | |
MMM
|M|M| |
MM
|M|M|M|
|M| |M|
M
|M|M|M|
| |M|M|

|M|M|M|
|M|M| |
|M| | |
| | | |
FIM!
