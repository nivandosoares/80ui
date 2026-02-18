# 80's DOS/Windows-style UI em C

Interface em modo texto inspirada em MS-DOS Shell / gerenciadores clássicos, com:

- barra de menu no topo;
- janelas com bordas e títulos;
- barra de status inferior com relógio;
- navegação totalmente por teclado.

## Requisitos

- compilador C (gcc/clang)
- `ncurses`

## Compilar

```bash
make
```

## Executar

```bash
make run
```

## Teclas

- `Tab`: alterna foco entre janelas
- `Setas`: navega itens
- `PgUp/PgDn`: rola listas grandes
- `Enter`: seleciona item ativo
- `F1`: ajuda rápida
- `Esc` ou `q`: sair

## Estrutura da interface

- **Topo**: título + menu (`File`, `Options`, `View`, `Tree`, `Help`)
- **Centro (esquerda)**: árvore de diretórios
- **Centro (direita)**: lista de arquivos
- **Inferior**: utilitários de disco
- **Rodapé**: atalhos e relógio

