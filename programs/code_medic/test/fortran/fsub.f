        subroutine fsub(VALUES, SUM)

        integer*4   VALUES(*)
        integer*4   SUM
        integer*4   I

        SUM = 0

        do 1 I = 1, 100
            SUM = SUM + VALUES(I)
1       continue

        return
        end
