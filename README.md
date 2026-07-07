# Survive & Fry

A cooking game developed for [GitHub's Game Off 2023](https://itch.io/jam/game-off-2023). The game draws inspiration from [Overcooked](https://ghosttowngames.com/game/overcooked), but with a unique twist where players cook meals to cure zombies instead of escaping from them.

## About

Survive & Fry was developed as a team project under Jammers Studio, designed and built in C++ on Unreal Engine within a one-month deadline for the Game Off 2023 jam. I served as the Game Designer and Gameplay Programmer. Players chop vegetables, combine ingredients, add an antidote, and scale portions before serving meals to incoming zombies in a time-pressure loop.

## How It Works

- Players follow a preparation pipeline from chopping to combining to serving
- Ingredient types are defined in a DataTable, so adding a new ingredient requires a new row and six lines of code with no changes to validation or interaction logic
- Shipped clean at the jam deadline and praised by the community for being smooth and well crafted

## Post-Jam Updates

Development continued after the jam with a focus on architecture:

- Match rules (timer, score, win/lose) moved from the player controller into a dedicated GameMode class
- Save system rebuilt in C++ with a thin Blueprint layer for asset references
- High scores now record elapsed match time, tracked separately from the countdown so serve bonuses and penalties do not distort the result
- Reworked inputs into proper action mappings

The playable build on itch.io is the original jam submission. The refactor lives in this repository.

## Built With

- Unreal Engine
- C++
- Visual Studio

## Getting Started

### Prerequisites

- Unreal Engine (version 5.4.4)
- Visual Studio
- Git

### Setup

1. Clone the repository

   ```
   git clone https://github.com/murahman-dev/Survive-and-Fry.git
   ```

2. Open the `.uproject` file in Unreal Engine

## Playable Build

Available on [itch.io](https://jammersstudio.itch.io/survive-fry).

## Walkthrough

- [Gameplay Video](https://www.youtube.com/watch?v=TlvjtERpfvo)

## Team

Developed under Jammers Studio:

- Riyad Hassan - Level and Props Design
- Muntaseer Hafiz Sazid - Narrative Design, UI Design, Environment Art
- Mesbah Ur Rahman - Game Design & Gameplay Programming

SFX from [freesound.org](https://freesound.org), background music from [purple-planet.com](https://www.purple-planet.com), and UI font Fredoka from [Google Fonts](https://fonts.google.com/specimen/Fredoka).

## License

This project is open-source under the [MIT License](LICENSE).

See [NOTICE](NOTICE.md) for the full third-party attribution.

## Contact

Mesbah Ur Rahman

- Email: mesbah@murahman.com
- LinkedIn: [linkedin.com/in/mesbah-ur-rahman997](https://www.linkedin.com/in/mesbah-ur-rahman997)