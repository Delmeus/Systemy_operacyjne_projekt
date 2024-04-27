[x] - wektor wskaźników
[x] - zmien z mapy na kolekcje
[x] - odrębny wątek do generowania klientów
[x] - running jako volatile

26.04
II etap
1. na trasie od dystrybutora do punktu obsługi klienci nie mogą się wyprzedzać 
2. klienci nie mogą wejsc na stanowisko obsługi póki jest ono zajęte - mają zatrzymywać się przed nim dopóki nie będzie wolne.

[ ] - sprawdzenie czy stacja jest zajęta, jeśli tak można ustawić pod nią, jeśli nie to nie
[ ] - naprawić wychodzenie z dystrybutora - aktualnie mogą na siebie nachodzić.
[ ] - inym rozwiazanie moze byc zwracanie predkosci klienta przed danym klientem