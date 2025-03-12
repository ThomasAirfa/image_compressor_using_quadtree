# **Digital Image Compression: Hierarchical Quadtree Coding** (EN)

## Description

This project implements grayscale image compression based on **hierarchical Quadtree coding**. It allows encoding an image with or without loss and decoding it back to its original form.

## Features

- **Encoding** an image into **QTC** format  
  - **Lossless**: `-c`  
  - **Lossy**: `-a`  
- **Decoding** a **QTC** file into a **PGM** image (`-u`)
- **Editing the segmentation grid** for both the decoder and encoder (`-g`)
- **Doxygen documentation** available in `docs/html/index.html`

## 🛠 Installation

1. Navigate to the project directory:
   ```bash
   cd /path/to/project
   ```
2. Compile the project:
   ```bash
   make
   ```
3. Generate the Doxygen documentation:
   ```bash
   make doxygen
   ```
4. Run the encoder or decoder with the desired option:
   ```bash
   ./bin/codec -c -i filename.pgm
   ```

## Command Line Options

| Option | Description |
|--------|-------------|
| `-c`   | Lossless encoding |
| `-a`   | Lossy encoding |
| `-u`   | Decode a **QTC** file into **PGM** |
| `-g`   | Edit the segmentation grid |
| `-h`   | Display help message |
| `-v`   | Verbose mode (detailed output) |
| `-i`   | Specify input file |
| `-o`   | Specify output file |

## Author

- **Guillotin Thomas**

---

---

# **Compression d'Images Numériques : Codage Hiérarchique par Quadtree** (FR)

## Description

Ce projet implémente une compression d'image en niveaux de gris basée sur un **codage hiérarchique par Quadtree**. Il permet d'encoder une image avec ou sans perte et de la décoder sous sa forme originale.

## Fonctionnalités

- **Encodage** d'une image au format **QTC**  
  - **Sans perte** : `-c`  
  - **Avec perte** : `-a`  
- **Décodage** d'un fichier **QTC** en image **PGM** (`-u`)
- **Édition de la grille de segmentation** pour le décodeur et l'encodeur (`-g`)
- **Documentation Doxygen** disponible dans `docs/html/index.html`

## 🛠 Installation

1. Placez-vous dans le répertoire du projet :
   ```bash
   cd /chemin/vers/le/projet
   ```
2. Compilez le projet avec :
   ```bash
   make
   ```
3. Générez la documentation Doxygen avec :
   ```bash
   make doxygen
   ```
4. Exécutez l'encodeur ou le décodeur avec l'option souhaitée :
   ```bash
   ./bin/codec -c -i nom_du_fichier.pgm
   ```

## Options de la ligne de commande

| Option | Description |
|--------|-------------|
| `-c`   | Encodage sans perte |
| `-a`   | Encodage avec perte |
| `-u`   | Décodage d'un fichier **QTC** en **PGM** |
| `-g`   | Édition de la grille de segmentation |
| `-h`   | Affichage de l'aide |
| `-v`   | Mode verbeux (affiche plus de détails) |
| `-i`   | Spécifie le fichier d'entrée |
| `-o`   | Spécifie le fichier de sortie |

## Auteur

- **Guillotin Thomas**
