# C-project - pipe, shared memory


## Opis [^desc]
Zestaw programów typu 'producent', 'konsument' prezentujący
możliwości synchronizacji i współbieżnego działania kilku
procesów zdolnych do obróbki i przekazywania sobie danych.
___
**System operacyjny:** *Linux*

**Język:** *C*

**Mechanizmy synchronizacji**: 
  - *łącza nienazwane (pipe),* 
  - *pamięć współdzielona (shmem).*


## Uruchomienie projektu: [^run] 

Przejście do katalogu projektu:
> cd *existing project folder*

Wywołanie głównego skryptu bash: *project.sh*:
> ./project.sh

Skrypt samodzielnie przeprowadzi kompilację wszystkich
plików źródłowych *.c* i w przypadku nie wystąpienia
błędów kompilacji uruchomi zestaw programów.

W przypadku wystąpienia błędów kompilacji skrypt wyświetli
zidentyfikowane błędy i zakończy działanie.


## Procesy [^proc]

### Proces główny - MAIN
  Z procesu głównego powoływane są 3 procesy potomne.
  Po ich powołaniu proces główny wstrzymuje pracę, aż do 
  ich zakończenia. Wówczas proces główny zamyka wszystkie
  uprzednio utworzone struktury synchronizujące procesy
  i kończy działanie.

### Proces potomny 1
  Czyta dane z *stdin* (standardowego wejścia) lub z *file* (pliku) oraz przekazuje
  je do procesu 2 w niezmienionej formie poprzez **łącze nienazwane** (pipe).

### Proces potomny 2
  Odebrane od procesu 1 dane konwertuje do postaci heksadecymalnej,
  a następnie umieszcza je w odpowiednim segmencie pamięci współdzielonej
  i informuje o tym proces 3. 

### Proces potomny 3
  Odczytuje dane z segmentu pamięci współdzielonej i wypisuje je 
  na *stderr* (standardowy strumień diagnostyczny).

## Sygnały [^sig]
  Operator może wysłać do dowolnego procesu następujące sygnały:
    - SIGINT  - zakończenie działania,
    - SIGUSR1 - wstrzymanie działania,
    - SIGUSR2 - wznowienie  działania.
    
  Proces odbierający  sygnał  musi powiadomić pozostałe procesy
  o otrzymanym sygnale.  Wówczas wszystkie procesy realizują
  odpowiednie operacje będące interpretacją odebranego sygnału.

[^desc]: Opis
[^run]: Uruchomienie projektu
[^proc]: Procesy
[^sig]: Sygnały
