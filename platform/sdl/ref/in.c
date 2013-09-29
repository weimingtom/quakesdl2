void install_grabs() {
}

void uninstall_grbs() {
}

static void Force_CenterView_f (void)
{
	in_state->viewangles[PITCH] = 0;
}

static void RW_IN_MLookDown (void) 
{ 
	mlooking = true; 
}

static void RW_IN_MLookUp (void) 
{
	mlooking = false;
	in_state->IN_CenterView_fp ();
}

void RW_IN_Init(in_state_t *in_state_p)
{
	int mtype;
	int i;

	in_state = in_state_p;

	// mouse variables
	m_filter = ri.Cvar_Get ("m_filter", "0", 0);
    in_mouse = ri.Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
    in_dgamouse = ri.Cvar_Get ("in_dgamouse", "1", CVAR_ARCHIVE);
	freelook = ri.Cvar_Get( "freelook", "0", 0 );
	lookstrafe = ri.Cvar_Get ("lookstrafe", "0", 0);
	sensitivity = ri.Cvar_Get ("sensitivity", "3", 0);
	m_pitch = ri.Cvar_Get ("m_pitch", "0.022", 0);
	m_yaw = ri.Cvar_Get ("m_yaw", "0.022", 0);
	m_forward = ri.Cvar_Get ("m_forward", "1", 0);
	m_side = ri.Cvar_Get ("m_side", "0.8", 0);

	ri.Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
	ri.Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

	ri.Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	mx = my = 0.0;
	mouse_avail = true;
}

void RW_IN_Shutdown(void)
{
	mouse_avail = false;
}

/*
===========
IN_Commands
===========
*/
void RW_IN_Commands (void)
{
}

/*
===========
IN_Move
===========
*/
void RW_IN_Move (usercmd_t *cmd)
{
	if (!mouse_avail)
		return;
   
	if (m_filter->value)
	{
		mx = (mx + old_mouse_x) * 0.5;
		my = (my + old_mouse_y) * 0.5;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mx *= sensitivity->value;
	my *= sensitivity->value;

// add mouse X/Y movement to cmd
	if ( (*in_state->in_strafe_state & 1) || 
		(lookstrafe->value && mlooking ))
		cmd->sidemove += m_side->value * mx;
	else
		in_state->viewangles[YAW] -= m_yaw->value * mx;

	if ( (mlooking || freelook->value) && 
		!(*in_state->in_strafe_state & 1))
	{
		in_state->viewangles[PITCH] += m_pitch->value * my;
	}
	else
	{
		cmd->forwardmove -= m_forward->value * my;
	}

	mx = my = 0;
}

static void IN_DeactivateMouse( void ) 
{
	if (!mouse_avail || !dpy || !win)
		return;

	if (mouse_active) {
		uninstall_grabs();
		mouse_active = false;
	}
}

static void IN_ActivateMouse( void ) 
{
	if (!mouse_avail || !dpy || !win)
		return;

	if (!mouse_active) {
		mx = my = 0; // don't spazz
		install_grabs();
		mouse_active = true;
	}
}

void RW_IN_Frame (void)
{
}

void RW_IN_Activate(qboolean active)
{
	if (active || vidmode_active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse ();
}
