
#pragma once
#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__




#endif //WEBSERV_LOGGER_HPP

// Объект в традиционном программировании - часть памяти, используемая для хранения данных. Объект в ООП - данные и поведение как единое целое
// Инкапсуляция - это процесс скрытого хранения деталей реализации объекта. Пользователи обращаются к объекту через открытый интерфейс
//



// прямая и uniform инициализации
// объявление и инициализация переменной непосредственно в месте, где она потом будет использоваться
// суффиксы после литералов: unsigned int = 5u, float = 5.0f
// int x = 012 - восьмиричный литерал, int x = 0x6F - шестнадцатеричный литерал, int x = 0b1010 - двоичный (С++14)
// с С++14 можно записать int x = 2'147'483'647
// константы const int должны быть проинициализированы при объявлении
// constexpr - ключевое слово для пометки переменной как константы времени компиляции для ускорения компиляции
// отдельная библиотека с константами constants.hpp
// алогритм сравнения дробных числе Дональда Кнута return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon)
// static (файловая область видимости), extern (внешняя область видимости), по умолчанию при объявлении конст - static, неконст - extern
// есть using-объявление (using std::cout), а есть using-директива (using namespace std), первое безопаснее и предпочтительнее, нужно использовать ТОЛЬКО внутри тела функций
// конвертацию C-style лучше избегать, так как не происходит проверки во время компиляции в отличие от static_cast
// typedef и возвращаемые значения для улучшения читабельности и понимания кода
// использовать type alias (using time_t = double) вместо typedef
// инициализация структуры Employee employee = { 4, 12, 43.12 } либо с С++ 11 Employee employee { 4, 12, 43.12 }
// тест 0 1 2 для проверки циклов
// с помощью sizeof() можно определить размер массива фиксированной длины, но только в той функции, где он объявлен
// const char *s = "text" имеет статическое время жизни, в отличие от char[] = "text"
// new (std::nothrow) для возврата 0, а не исключения при неудачном выделении памяти
// длина динамического массива должна быть явно указана при инициализации
// фундаментальные типы данных можно передавать не поссылке в функцию
// цикл foreach: for (const auto &elem : array) для перебора массива фикс размера либо vector, list, etc.
// фиксированные массивы распадаются на указатели при передачи в функцию
// параметры функции по умолчанию должны находиться справа
// int (*ptr)() - указатель на функцию int func()
// указатели на функции не работают со значеними по умолчанию
// для обработки критических ошибок ВО ВРЕМЯ ДЕБАГА можно использовать assert(bool), если false он завершит программу и выдаст утверждение, номер строки и имя файла
// static_assert - работает на этапе компиляции
// std::stringstream >> int конвертация из строки в число
// два объекта класса имеют доступ к приватным членам друг друга
// в классе сначала инициализируются переменные
// функция-метод Init() для инициализации объекта параметрами по умолчанию, рекомендуется для инициализации в конструкторе

// RAII идиома (Resource Acquisition Is Initialization - Приобритение ресурса есть инициализация)
// - это идиома ООП, при которой использование ресурсов привязывается к времени жизни объектов с автоматической продолжительностью жизни

// при exit() деструкторы не вызываются
// для инициализации статических переменных класса можно воспользоваться вложенным статическим классом и его конструктором
// Class(value) - анонимный объект класса
// Дружественные классы - это классы, в которых все методы являются дружественными
// 3 способа перегрузки операторов: дружественный, обычный, методы
// Операторы = [] () -> могут быть перегружен только как методы класса
// Оператор << только как дружественный
// Функторы - классы работающие как функции, но еще и хранят данные
// можно перегружать оператор преобразования типов: operator int() { return value; }
// Элизия - процесс прямой инициализации объекта анонимным объектом (конструктор копирования не вызывает), но конструктор копирования при этом должен быть открытым
// Поэтому нужно избегать копирующей инициализации для классов

// Явные конструкторы с ключевым словом explicit не используются для неявных конвертаций,
// при этом явное преобразование через cast разрешено также как и при прямой инициализации Class('a')

// delete для запрета определенных конструкторов: Class(char) = delete;
// std::initializer_list - класс для реализации списка инициализации для пользовательского класса

// Композиция: композиция (часть чего то, человек и сердце), агрегация (имеет, человек и адрес),
// ассоциация (использует, человек и автомобиль), зависимость (один класс использует другой для выполнения задания)

// Наследование: конструкторы дочерних классов могут вызывать конструкторы ТОЛЬКО родительских классов, но не выше по иерархии
// protected-члены - если количество дочерних классов небольшое и классами пользуюсь только я
// наследование по умолчанию - private
// спецификатор доступа при наследовании влияет ТОЛЬКО на внешний доступ к родительским членам через дочерний класс
// методы дочернего класса не наследуют спецификаторы доступа

// дочерний класс может изменить спецификатор доступа метода родительского класса с public или protected используя using-объявление
// или наоборот скрыть доступ к родительским членам таким же образом, так же можно удалить ненужные методы через delete

// Виртуальная функция - это особый тип функции, который при ее вызове, выполняет наиболее дочерний метод, который существует между родительским и дочерними классами.
// override пишется в конце объявления функции, которую хотим переопределить для проверки этого во время компиляции
// final в конце имени функции или класса если хотим чтобы цепочка наследования/переопределения не продолжалась
// Ковариантный тип возврата - когда у дочернего класса возврат Child*, у родителя Parent*

// Раннее связывание - обычные функции (статическая привязка)
// Позднее связывание - указатели на функции (динамическая привязка)

// __*vptr - указатель на виртуальную таблицу, появляется в базовом классе

// для чисто виртуальной функции можно определить поведение по умолчанию вне класса

// наследники обязаны переопределять чисто виртуальную функцию, иначе они так же будут абстрактными

// Интерфейс - класс не имеющий переменных-членов, а все методы чисто виртуальные

// единственный случай когда дочерний класс может вызвать конструктор класса деда - ромбовидное наследование

// Parent = Child обрезка объектов

// std::vector<Class&> ЗАПРЕЩЕНО, объекты должны быть переприсваемыми
// Решение std::reference_wrapper - класс, работающий как ссылка на объект, но реализующий переприсваивание (не может быть анонимным)


// dynamic_cast не работает при наследовании private, protected; при отсутствии виртуальных таблиц
// dynamic_cast также работает со ссылками

// реализацию методов шаблоннного класса нужно указывать в том же месте, где определяются типы шаблона (нельзя разбить традиционно на .hpp и .cpp)
// либо применить метод подключения реализации в .hpp файле через файл .inl либо метод 3 файлов: class.h class.cpp templates.cpp

// шаблон может содержать non-type параметр (int, char..)

// запрещено частично специализировать шаблон функции, но не класса

// Раскручивание стека: процесс когда исключение ищет блок try/catch переходя от caller'a к caller'у

// catch-all: catch(...) - ловит все

// void func() noexcept - функция гарантирует НЕ выбрасывать исключения (с С++11)
// noexcept позволяют компилятору выполнять некоторые оптимизации

// исключения ловятся ТОЛЬКО блоком try!

// для повторного выброса исключения того же типа просто пишем throw; без указания конкретного исключения

// функциональный блок try-catch (в основном с конструкторами): B() try : A() {} catch (...) {}
// нельзя выполнить полную обработку, можно только сгенерировать новое исключение, либо перекинуть это же выше в caller


// std::unique_ptr - выходя из области видимости сам удаляет ресурсы хранимые в себе

// исключения дорогие, как при норм работе так и при ошибках
// есть Исключения zero-cost - незаметные при норм работе, и намного дороже при ошибках

// Условие для выброса исключения:
// 1. Ошибка возникает редко
// 2. Программа не может продолжить свое выполнение без обработки ошибки
// 3. Ошибка не может быть обработана в том месте, где возникает
// 4. Нет альтернативного хорошего способа вернуть ошибку обратно в caller

// Ссылки r-value (только значения r-value): int &&ref = 7;

// std::move для перемещения, без копирования

// std::move_backward() ???

// использовать std::make_unique() для создания умного указателя, так как она также обработает возможные исключения

// 3 типа контейнеров: последовательный (vecto, list, deque), ассоциативный (set, map, multimap), адаптер (stack, queue, priority_queue)

// tellg() - сообщает текующую позицию (в байтах) файлового указателя

// std::string_view для неизменяемых строк (не производится копирование при инициализации)

// std::string не использует \0

// алгоритмы делятся на 3 типа: инспекторы (просмотр/поиск элементов), мутаторы (сортировка), фасилитаторы (генерация результатов на основе значений контейнера)

// проблемы ромбовидного наследования ???

// почему крайне не рекомендуется использовать auto для возвращаемых значений функций ???

// trailing: auto func() -> int   новый способ указания типа возвращаемого значения

// auto ???

// [захват](параметры) {тело} -> return_type лябмда функция
// в захвате создаются клоны переменных для лямбда функции
// [value] () mutable {} для захвата по значению
// [&value] () {} захват по ссылке
// [=] () {} захват всех переменных по значению
// [=, &value] () {} захват value по ссылке, остальные по значению
// [&, value] () {} захват value по значению, остальные по ссылке
// [value {a * b}] () {} value - временная переменная типа возвращенного после произведения a * b (например int если int a && int b)
// захваченные переменные должны существовать дольше, чем сама лямбда

// Библиотеки импорта - инструмент для упрощенного использования динмаических библиотек (подключаются как статические)

// decltype ??? C++ 11

// std::tuple C++ 11

// атрибут deprecated ??? C++ 14

// volatile сообщает компилятору, что значение переменной МОЖЕТ меняться извне
// т.е. запрещает компилятору опитимизировать операции с участием этой переменной




