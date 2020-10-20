with Text_IO; use Text_IO;
with pkg_tarea; use pkg_tarea;
with Ada.Real_Time; use Ada.Real_Time;
with Ada.Task_Identification;

procedure main is
    Tarea1: tarea_periodica_t;
    Tarea2: tarea_periodica_t;
    begin
        Put_Line("----------------Inicio Main---------------");
        delay 8.0;
        abort Tarea1;
        abort Tarea2;
        delay 2.0;
        Put_Line("-----------------Fin Main-----------------");

end main;
