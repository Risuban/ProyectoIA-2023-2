# ProyectoIA-2023-2

Para crear el ejecutable se abre una consola en la dirección correspondiente y se utiliza el comando 

```
make
```

Una vez el ejecutable sea creado, se debe ejecutar  con 

```
.\mi_programa.exe <ruta archivo tipo 1> <ruta archivo tipo 2> <n de restarts> 
```

en windows o para linux

```
mi_programa.exe <ruta archivo tipo 1> <ruta archivo tipo 2> <n de restarts> 
```

Sobre 15 restarts el codigo comenzara a tomar más de 1 minuto en ejecutar, pero tendrá mejores resultados.

Además dentro del código el número de iteraciones y el largo de la lista tabú están hardcodeados pero se pueden cambiar, ambos están dentro de main en las lineas 292 y 293. Se recomienda no usar mas de 5000 iteraciones.