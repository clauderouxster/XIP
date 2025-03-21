inv={}
inv["sg"]="pl"
inv["pl"]="sg"
node={"sg":"+","lem":"toto"}
fx="pl"
if fx in ["sg","pl"]:
    if fx not in node:
        if inv[fx] in node:
            node.pop(inv[fx])
        node[fx]="+"
print node
