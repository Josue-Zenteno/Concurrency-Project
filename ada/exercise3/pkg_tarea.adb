with Ada.Real_Time; use Ada.Real_Time;
with Ada.Text_IO; use Ada.Text_IO;
with Ada.Task_Identification; use Ada.Task_Identification;

package body pkg_tarea is

   task body tarea_periodica_t is

        T : Integer;
        Proximo_Periodo: Time := Clock;
        InitEjecucion: Time;
        Periodo : constant Time_Span := Milliseconds(2000);
        CurrentInstant: Duration;
        
        begin

            --delay To_Duration(1);
            delay 1.0;
            InitEjecucion := Clock;
            T := 0;

            while T < 1000 loop

                CurrentInstant := To_Duration(Clock - InitEjecucion);
                T := Integer(CurrentInstant);

                Put_Line("Tarea("&Image(Current_Task)&"):Variable interna: " & Integer'Image(T));

                Proximo_Periodo := Proximo_Periodo+Periodo;
		        delay until Proximo_Periodo;

            end loop;

   end tarea_periodica_t;

end pkg_tarea;