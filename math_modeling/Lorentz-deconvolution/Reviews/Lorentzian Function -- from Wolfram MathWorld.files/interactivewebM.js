// Should match JSPRoot given in build template.
var JSPRoot = "http://mathworld.wolfram.com/webMathematica/";

function EscapeP(s)
// Escapes '+', which the JavaScript
// function escape() does not do.
{
	var t=escape (s);
	return t.replace(/\+/g,"%2B");
}

function showComplexPlots(name,ReMin,ReMax,ImMin,ImMax)
{
	// Create URL for plot
	var r;
	var n = JSPRoot + "ComplexPlots.jsp";
	r=n+"?name="+
		escape(name)+"&zMin="+EscapeP(ReMin)+"%2B"+
		EscapeP(ImMin)+"*I"+"&zMax="+EscapeP(ReMax)+"%2B"+
		EscapeP(ImMax)+"*I" + "&nt=1";

	return r;
}

function showRealLinePlots(name,xMin,xMax)
{
	var r;
	var n = JSPRoot + "RealLinePlots.jsp";

	r=n+"?name="+ 
		escape(name)+"&xMin="+EscapeP(xMin)+"&xMax="+
		EscapeP(xMax) + "&nt=1";

	return r;
}

function showNumberTheoryPlots(name,xMin,xMax) 
{
	var r;   
	var n = JSPRoot + "NumberTheoryPlots.jsp";   

	r = n+"?name="+escape(name)+"&xMin="+EscapeP(xMin)+"&xMax="+             
		EscapeP(xMax) + "&nt=1";

	return r;
}

function showIntegerPlots (name,xMin,xMax) 
{
	var r;
	var n = JSPRoot + "IntegerPlots.jsp";

	r=n+"?name="+escape(name)+"&xMin="+EscapeP(xMin)+"&xMax="+
             EscapeP(xMax) + "&nt=1";
	
	return r;
}

function showRiemannSum (fun,xMin,xMax,n,sample,estimate,rectanglestyle,plotstyle) 
{
	var r,show;   
	var jspname = JSPRoot + "RiemannSum.jsp";   
	if (estimate)
		show="True";   
		else show="False";   
	r= (jspname+"?fun="+EscapeP(fun)+"&xMin="+
                EscapeP(xMin)+"&xMax="+
                EscapeP(xMax)+"&n="+
                EscapeP(n)+"&sample="+
                EscapeP(sample)+"&estimate="+
                show+"&rectstyle="+rectanglestyle+
		"&plotstyle="+plotstyle + "&nt=1");

	return r;
}
