## Imports
```console
sudo apt install libreadline-dev
```

## Captures d'écran
Les captures d'écran "valgrind" et "ps" permettent de voir la bonne terminaison du shell : pas de perte de mémoire et de processus zombie.

## Objdump / nm
Vérification de la table des symboles :

```console
objdump -t biceps
nm biceps.o
nm gescom.o
```

Note : interne (t,b,d), externe (T,D,B), Undefined (U)

