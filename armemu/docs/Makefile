# This is the Makefile for the demonstration dissertation
# written by Martin Richards
#
# Note that continuation lines require '\'
# and that TAB is used after ':' and before unix commands.

# USAGE:
#
# make                     -- to paginate the dissertation
#
# make proposal.dvi        -- to paginate  proproposal
#
# make xdvi                -- view the dissertation with xdvi
#
# make diss.ps             -- make a postscript version of the dissertation
#
# make gv                  -- view the dissertation with ghostview
#
# make all                 -- construct proposal.dvi, diss.dvi and diss.ps
#
# make clean               -- remove all remakeable files

DISS = diss.tex title.tex proforma.tex \
	ch1.tex ch2.tex ch3.tex ch4.tex ch5.tex \
	refs.bib originality.tex \
	apa.tex apb.tex apc.tex apctable.tex pheta.tex \
	proposal.tex propbody.tex intexampletable.tex \
	intermeddefs.tex addrule.eps subimmrule.eps tree.eps \
	profile.eps stmgraph.eps structure.eps

run:	pass2

again:	pass3
	latex diss

pass3:	pass2
	latex diss
	touch pass3

pass2:	pass1
	latex diss
	touch pass2

pass1:	$(DISS)
	latex diss
	bibtex diss
	touch pass1

proposal.dvi: proposal.tex propbody.tex timescale.tex
	latex proposal

all:	proposal.dvi diss.ps

clean:
	rm -f diss.ps *.dvi *.aux *.log *.err
	rm -f core *~ *.lof *.toc *.blg *.bbl
	rm -f *.lof *.toc *.blg *.bbl
	rm -f pass1 pass2 pass3

xdvi:	pass3
	xdvi diss.dvi

gv:	diss.ps
	ghostview diss.ps

diss.ps:	pass3
	dvips diss.dvi -o diss.ps

diss-odd.ps:	pass3
	dvips -A diss.dvi -o diss-odd.ps

diss-even.ps:	pass3
	dvips -B diss.dvi -o diss-even.ps

online:	diss.ps
	scp diss.ps isobel:/home/jules/www
