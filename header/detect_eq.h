//define local variable(s) here.  Use static keyword to keep local, e.g:
//   static int i;  // defines a local int named i



/*complete the state machine*/

void Detect_EQ()
{
    switch(detect_eq_state)
    {
        case DEQInit:
            //init variable(s) here.
            break;
        default:
            detect_eq_state = DEQInit;
            break;
    }
    switch(detect_eq_state)
    {
        case DEQInit:
            break;
        default:
            break;
    }
}
