package pkg_sem is

   type sem_t is limited private;

   procedure wait(sem : in out sem_t);
   procedure signal(sem : in out sem_t);

private

   protected type sem_t(valor_inicial : integer := 0) is

      entry wait;
      entry signal;

   private

      valor_sem : integer := valor_inicial;

   end sem_t;


end pkg_sem;
