# TP1 - Modélisation Géométrique

Ce TP implémente les fonctionnalités de base pour le chargement, l'affichage, la manipulation et la subdivision de maillages 3D en s'appuyant sur la structure de données Half-Edge (demi-arêtes).

---

## Fonctionnalités implémentées

### Lecture du fichier (`readFile`)
On extrait les sommets et on monte la structure Half-Edge en reliant chaque arête à sa jumelle (`twin`). Ça crée le squelette propre du modèle 3D dès le chargement du fichier.

### Calcul des normales (`computeNormals`)
C’est ce qui donne du relief à l’objet sous la lumière. On calcule l'orientation de chaque face, puis on fait la moyenne sur les sommets pour que l’ombrage soit bien lisse au rendu.

### Calcul de la Silhouette (`Silhouette`)
On cherche les arêtes à la frontière de l'objet par rapport à là où regarde la caméra. En comparant l'orientation des deux faces de l'arête, on trace le contour du modèle en rouge.

### Triangulation des faces (`triangulation`)
On découpe les polygones complexes du fichier en triangles basiques à partir d’un point central. C'est obligatoire pour qu'OpenGL affiche le maillage sans faire n'importe quoi.

### Surface de révolution (`SurfaceDeRevolution`)
On dessine un profil de points en 2D, on choisit un axe, et le programme le fait tourner sur 360 degrés. Ça génère automatiquement une forme 3D complète modélisée en quads.

### Simplification de maillage (`mesh simplification`)
L'algo cherche en boucle l'arête la plus courte du modèle pour fusionner ses sommets. Ça permet d'alléger le fichier en virant des polygones tout en gardant la forme globale.

### Subdivision de Catmull-Clark (`Catmull-Clark`)
Notre outil pour lisser et arrondir le maillage en divisant chaque face en quatre nouveaux quads. L’algo marche bien, même si ça peut devenir un peu instable ou sauter selon la tronche du maillage de départ.
