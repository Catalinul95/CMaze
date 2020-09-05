#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>

#define ROWS 15
#define COLS 20
#define SIZE 32

typedef struct {
  unsigned int row;
  unsigned int col;
  unsigned right_side;
  unsigned bottom_side;
  unsigned int visited;
} Cell;

typedef struct {
  Cell *data;
  unsigned int size;
  unsigned int index;
  unsigned int peep;
} Stack;

unsigned int get_random_int(const int max) {
  SDL_Delay(50);
  unsigned int r = rand() % max;

  return r;
}


void stack_create(Stack *stack, const unsigned int size) {
  stack->data = malloc(sizeof(Cell) * size);
  stack->index = -1;
  stack->size = size;
  stack->peep = size;

  if (stack->data == NULL) {
    printf("Unable to allocate memory for stack! \n");
    exit(1);
  }
}

void stack_add(Stack *stack, Cell *cell) {
  if ((stack->index + 1) == stack->size) {
    printf("The stack is full! \n");
    exit(1);
  }

  stack->data[++stack->index] = *cell;

  stack->peep = stack->index;
}

Cell* stack_pull(Stack *stack) {
  if (stack->index == -1) {
    printf("The stack is empty! \n");
    exit(1);
  }

  return &stack->data[stack->index--];
}

Cell* stack_peep(Stack *stack) {
  if (stack->index == -1) {
    printf("The stack is empty! \n");
    exit(1);
  }

  return &stack->data[stack->peep--];
}

Cell* stack_find(Stack *stack, const unsigned int row, const unsigned int col) {
  if (stack->index == -1) {
    printf("The stack is empty! \n");
    exit(1);
  }

  for (int i = 0; i < stack->size; i++) {
    if (stack->data[i].row == row && stack->data[i].col == col) {
      return &stack->data[i];
    }
  }
}

void cells_create(Stack *stack) {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      Cell cell;
      cell.row = y;
      cell.col = x;
      cell.visited = 0;
      cell.right_side = 1;
      cell.bottom_side = 1;
      stack_add(stack, &cell);
    }
  }
}

unsigned int stack_has(Stack *stack, const unsigned int row, const unsigned int col) {
  if (stack->index == -1) {
    printf("The stack is empty! \n");
    exit(1);
  }

  for (int i = 0; i < stack->size; i++) {
    if (stack->data[i].row == row && stack->data[i].col == col) {
      return 1;
    }
  }

  return 0;
}

unsigned int has_free_neighbours(Stack *stack, Cell *cell) {
  unsigned int neighbours_count = 0;

  if (stack_has(stack, cell->row - 1, cell->col) && 
    stack_find(stack, cell->row - 1, cell->col)->visited == 0) {
    neighbours_count++;
  }

  if (stack_has(stack, cell->row + 1, cell->col) && 
    stack_find(stack, cell->row + 1, cell->col)->visited == 0) {
    neighbours_count++;
  }

  if (stack_has(stack, cell->row, cell->col - 1) && 
    stack_find(stack, cell->row, cell->col - 1)->visited == 0) {
    neighbours_count++;
  }

  if (stack_has(stack, cell->row, cell->col + 1) && 
    stack_find(stack, cell->row, cell->col + 1)->visited == 0) {
    neighbours_count++;
  }

  return neighbours_count;
}

Cell* pick_neighbour(Stack *stack, Cell *cell) {
  Cell *neighbours[4];
  unsigned int neighbours_count = 0;

  if (stack_has(stack, cell->row - 1, cell->col) && 
    stack_find(stack, cell->row - 1, cell->col)->visited == 0) {
    neighbours[neighbours_count++] = stack_find(stack, cell->row - 1, cell->col);
  }

  if (stack_has(stack, cell->row + 1, cell->col) && 
    stack_find(stack, cell->row + 1, cell->col)->visited == 0) {
    neighbours[neighbours_count++] = stack_find(stack, cell->row + 1, cell->col); 
  }

  if (stack_has(stack, cell->row, cell->col - 1) && 
    stack_find(stack, cell->row, cell->col - 1)->visited == 0) {
    neighbours[neighbours_count++] = stack_find(stack, cell->row, cell->col - 1);
  }

  if (stack_has(stack, cell->row, cell->col + 1) && 
    stack_find(stack, cell->row, cell->col + 1)->visited == 0) {
    neighbours[neighbours_count++] = stack_find(stack, cell->row, cell->col + 1);
  }

  unsigned int r = get_random_int(neighbours_count);

  if (neighbours[r]->row > cell->row) {
    cell->bottom_side = 0;
  }

  if (neighbours[r]->row < cell->row) {
    neighbours[r]->bottom_side = 0;
  }

  if (neighbours[r]->col > cell->col) {
    cell->right_side = 0;
  }

  if (neighbours[r]->col < cell->col) {
    neighbours[r]->right_side = 0;
  }

  return neighbours[r];
}

void main() {
  srand(time(NULL));

  SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("SDL2 Maze generation algorithm",
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    1024, 640, 0
                    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    unsigned int visited_cells = 0;

  // where we keep track of visited cells
  Stack stack;
  stack_create(&stack, ROWS * COLS);

  // the actual cells
  Stack cells;
  stack_create(&cells, ROWS * COLS);

  // create cells
  cells_create(&cells);

  Cell *cell = stack_find(&cells, 0, 0);
  stack_add(&stack, cell);
  cell->visited = 1;
  visited_cells++;


  while (visited_cells < (ROWS * COLS)) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer);

      if (has_free_neighbours(&cells, cell) > 0) {
      cell = pick_neighbour(&cells, cell);
      cell->visited = 1;
      stack_add(&stack, cell);
      visited_cells++;
    } else {
      cell = stack_peep(&stack);
    }

    SDL_Event event;

    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }
    }

    
    for (int y = 0; y < ROWS; y++) {
      for (int x = 0; x < COLS; x++) {
        SDL_Rect rect;
        rect.x = x * (SIZE + 5);
        rect.y = y * (SIZE + 5);
        rect.w = SIZE;
        rect.h = SIZE;

        Cell *cell = stack_find(&cells, y, x);

        if (cell->bottom_side == 0) {
          rect.h += 5;
        }

        if (cell->right_side == 0) {
          rect.w += 5;
        }

        if (cell->visited) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        }
        
        SDL_RenderFillRect(renderer, &rect);

        
      }
    }

    // for (int y = 1; y <= ROWS; y++) {
    //  for (int x = 1; x <= COLS; x++) {
    //    SDL_Rect line1;
    //    line1.x = (x - 1) * (SIZE + 5);
    //    line1.y = y * SIZE;
    //    line1.w = SIZE;
    //    line1.h = 5;

    //    if (y > 1) {
    //      line1.y += 5 * (y - 1);
    //    }

    //    SDL_Rect line2;
    //    line2.x = x * SIZE;
    //    line2.y = (y - 1) * (SIZE + 5);
    //    line2.w = 5;
    //    line2.h = SIZE;

    //    if (x > 1) {
    //      line2.x += 5 * (x - 1);
    //    }



    //    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    //    SDL_RenderFillRect(renderer, &line1);
    //    SDL_RenderFillRect(renderer, &line2);
    //  }
    // }


    SDL_RenderPresent(renderer);
  }

  SDL_Delay(5000);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  free(cells.data);
  free(stack.data);
}
