# Turbo C++ style UI em C (ncurses)

Refeito do zero para parecer a interface clássica do **Turbo C++** em modo texto:

- barra de título e menu no topo;
- editor principal com numeração de linhas;
- painel de projeto à direita;
- painel de mensagens/compilação na parte inferior;
- barra de status com atalhos e relógio.

## Requisitos

- compilador C (`gcc`/`clang`)
- `ncurses`

## Compilar

```bash
make
```

## Executar

```bash
make run
```

## Atalhos

- `F10`: abre/fecha menu superior
- `F6`: alterna foco (editor/projeto/message)
- `Setas`: navega no painel com foco
- `PgUp/PgDn`: rola o editor
- `F9`: simula compilação
- `Ctrl+F9` (`KEY_F(21)`): simula execução
- `F1`: ajuda rápida
- `Esc`: sair

## Observações

- O layout foi pensado para terminal mínimo **90x24**.
- É uma interface funcional de demonstração, com foco em navegação por teclado e visual retro.
