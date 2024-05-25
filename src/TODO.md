[x] - wektor wskaźników
[x] - zmien z mapy na kolekcje
[x] - odrębny wątek do generowania klientów
[x] - running jako volatile

26.04
II etap
1. na trasie od dystrybutora do punktu obsługi klienci nie mogą się wyprzedzać 
2. klienci nie mogą wejsc na stanowisko obsługi póki jest ono zajęte - mają zatrzymywać się przed nim dopóki nie będzie wolne.

[x] - sprawdzenie czy stacja jest zajęta, jeśli tak można ustawić pod nią, jeśli nie to nie
[x] - naprawić wychodzenie z dystrybutora - aktualnie mogą na siebie nachodzić.
[x] - naprawic pętle gorącego czekania, żeby kliencie nie próbowali caly czas czy stacja jest wolna, tylko żeby czekali aż się zwolni

24.05
III etap
Do przejazdu przez dystrybutor konieczna jest jedna sekunda, w tym czasie dystrybutor jest zamknięty i inny klient nie moze do niego wejsc. Klient ma wyjść na kierunek pierwotnie wskazywany przez dystrybutor, nie na wskazywany przy wyjsciu.