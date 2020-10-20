with Text_IO; use Text_IO;

package body pkg_task is

    task body tarea_t is

        NumParConsult: Integer:= 0;
        NumImparConsult: Integer:= 0;

        begin

            loop

                select

                accept EsPar(N: in Integer) do

                    if N rem 2 = 0 then
                        NumParConsult:= NumParConsult + 1;
                        Put_Line("The number["& Integer'Image(N) &"]is even");
                    else
                        NumImparConsult:= NumImparConsult + 1;
                        Put_Line("The number["& Integer'Image(N) &"]is odd");
                    end if;

                end EsPar;

                or

                    accept EstadoConsulta do

                        Put_Line("The number of even numbers consulted is ["&Integer'Image(NumParConsult)&"]");
                        Put_Line("The number of odd numbers consulted is ["&Integer'Image(NumImparConsult)&"]");

                    end EstadoConsulta;
            
                end select;
                
            end loop;

    end tarea_t;

end pkg_task;
