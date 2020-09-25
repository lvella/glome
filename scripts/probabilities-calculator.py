import random

SEGMENTS = 20

resistance = []

for i in range(1000000):
    if i % 1000 == 0:
        print(i)
    radius = max(0.003, random.gauss(0.011, 0.0045))
    fragility_mean = random.gauss(1.5 * SEGMENTS, 0.5 * SEGMENTS)
    density = max(1500.0, random.gauss(6000.0, 2000.0))
    count = SEGMENTS * int(round(radius * density))

    damage = 0
    pieces_to_kill = 0

    while damage <= (0.6 * count):
        damage += max(2, int(round(random.gauss(fragility_mean, fragility_mean * 0.1))))
        pieces_to_kill += 1

    resistance.append(pieces_to_kill)

mu = sum(resistance)/len(resistance)
sigma = (sum((mu - x)**2 for x in resistance) / len(resistance))**0.5
print("Average resistance:", mu)
print("Standard deviation:", sigma)

import numpy as np
import matplotlib.pyplot as plt

# the histogram of the data
n, bins, patches = plt.hist(resistance, 2000, density=True)

plt.xlabel('Fragments to die')
plt.ylabel('Probability')
plt.axis([0, 150, 0, 0.07])
plt.grid(True)

plt.show()
