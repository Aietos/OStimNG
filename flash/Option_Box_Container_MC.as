﻿
class Option_Box_Container_MC extends MovieClip
{


	public function Option_Box_Container_MC()
	{
		// constructor code
	}
	
	public function setPosition(xPos: Number, yPos: Number, xScale: Number, yScale: Number)	{		
		this._x = xPos + 160;
		this._y = yPos + 980;
		this._xscale = xScale;
		this._yscale = yScale;
	}
}