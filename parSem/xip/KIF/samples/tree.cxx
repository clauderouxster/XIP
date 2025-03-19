int xz=0;

//Recursive traversing of a tree
function treedisplay(tree t) {
	if (t==null)
		return;
	xz+=1;
	print(t," ");
	if (t.child()!=null) {
		print("(");
		treedisplay(t.child());
		print(")");
		
	}
	treedisplay(t.next());
	xz-=1;
	if (xz==0)
		println();
}

//we create five nodes, with numerical values
tree test1(1);
tree test2(2);
tree test3(3);
tree test4(4);
tree test5(5);

test1.addchild(test2);
test1.addchild(test3);
test2.addchild(test4);
test4.addnext(test5);

treedisplay(test1); //we display now: 1 (2 (4 5 )3 )

//We modify the value of t5
test5=[100];

treedisplay(test1); //we display now: 1 (2 (4 [100] )3 )
test3.addnext(test2);
treedisplay(test1); //we display now: 1 (3 2 (4 [100] ))

//we remove test4
test4.remove();
treedisplay(test1); //we display now: 1 (3 2 ([100] ))

int cpt=test1+test2+test3;
println(cpt); //we display 6

tree test6(13);
tree test7(14);
tree test8(15);
tree test9(16);
tree test10(17);
tree test11(18);
tree test12(19);
tree test13(20);


test1.addchild(test2);
test1.addchild(test3);
test1.addchild(test4);
test1.addchild(test5);
test1.addchild(test6);

test5.addchild(test7);
test5.addchild(test8);
test5.addchild(test9);
treedisplay(test1);


test6.addchild(test7);
test6.addchild(test8);
test6.addchild(test9);

treedisplay(test1);

test8.addprevious(test10);
treedisplay(test1);

test8.addnext(test11);
treedisplay(test1);

test4.addchild(test6);
treedisplay(test1);



test9.remove();
treedisplay(test1);

test6=100;
treedisplay(test1);

if (test1.child(test2))
	println("Bien test1 child");
if (test8.previous(test10))
	println("Bien test8 previous");
if (test8.next(test11))
	println("Bien test8 next");

int xii=test5+test6;
println(xii);


treedisplay(test1);

self u;
while (u in test1)
	print(u,"[",u.depth(),"] ");
println();

riterator itt=test1;
for (itt.begin();itt.nend();itt.next())
	print(itt.value()," ");
println();
