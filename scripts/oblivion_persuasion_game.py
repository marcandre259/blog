# Fill in a multiplier matrix
from dataclasses import dataclass
from typing import List
import numpy as np
from itertools import permutations

T = 4
# Can change the starting count to modify the multipliers
cnt = 0

multiplier_matrix = np.zeros((T, T))
# The multipliers start at 1 and end at 4

for i in range(T):
    for j in range(T):
        multiplier_matrix[i][j] = (j + cnt) % 4 + 1
    cnt += 1

print(multiplier_matrix)


@dataclass
class RoundResult:
    time: int
    action: int
    multiplier_score: int
    score: int


@dataclass
class GameResult:
    round_results: List[RoundResult]
    game_result: int


# A are action, basically 4 actions and 4 times
# Actions are associated with a score (character preference)
action_map = {0: -2, 1: -1, 2: 1, 3: 1}

game_results = []

for game in permutations(action_map.keys()):
    game_rounds = []
    round_cnt = 0
    for action in game:
        round_score = action_map[action] * multiplier_matrix[round_cnt][action]
        round_result = RoundResult(
            round_cnt,
            action,
            float(multiplier_matrix[round_cnt][action]),
            float(round_score),
        )
        game_rounds.append(round_result)
        round_cnt += 1

    game_score = sum([game_round.score for game_round in game_rounds])
    game_results.append(GameResult(game_rounds, game_score))

# Find winning game
best_score = -np.inf
best_game = 0

for i, game in enumerate(game_results):
    if game.game_result > best_score:
        best_score = game.game_result
        best_game = i

print(game_results[best_game])

# action_score = action_map[a] * multiplier_matrix[t][a]
