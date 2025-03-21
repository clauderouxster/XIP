<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<html>
  <head>
    <meta name="generator" content="HTML Tidy, see www.w3.org">

    <title>Dependencies nomenclature of the XIP French
    Grammar</title>
  </head>

  <body>
  <a href="page1.jsp"> Back </a>
    <center>
      <h1>Dependencies nomenclature of the XIP French Grammar</h1>
    </center>
    <b>Author :</b> S. Ait-Mokhtar 

    <h1>Introduction</h1>
    The purpose of this document is to list the dependencies that
    are extracted by the XIP French grammar. A XIP dependency is a
    unary or binary predicate that binds one or two nodes together
    from the chunk tree. Usually, dependencies apply on lexical
    nodes. Each dependency is named with a specific label (subject,
    object, etc.) A dependency label denotes the grammatical
    relation that has been computed between the nodes that have
    been bound together. In a binary dependency relation, the first
    element is the head, and the second is the dependent. 

    <h1>List of dependencies</h1>
    Below is the list of dependencies extracted by the French XIP
    parser. The dependency features (starting with the underscore
    character) are explained in the next section. 

    <ul>
      <li>
        <h3>ADJMOD</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches the modifier of an adjective to
        the adjective itself. 

        <dl>
          <dt>Example</dt>

          <dd><i>Des moyens sup&eacute;rieurs &agrave; ceux du
          maire.</i></dd>

          <dd>
            <h5>ADJMOD(sup&eacute;rieurs,ceux)</h5>
          </dd>

          <dd><i>Impossible en fait de ne pas envisager de
          n&eacute;gociations.</i></dd>

          <dd>
            <h5>ADJMOD(impossible,envisager)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>AUXIL</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches an auxiliary to the verb it
        modifies. 

        <dl>
          <dt>Example</dt>

          <dd><i>Ils l'ont vu.</i></dd>

          <dd>
            <h5>AUXIL(vu,ont)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>CONNECT</h3>
      </li>

      <li style="list-style: none">
        This dependency links the verb of a finite clause to the
        grammatical word that introduces the clause. 

        <dl>
          <dt>Example</dt>

          <dd><i>Il semble peu probable que les
          d&eacute;put&eacute;s votent le projet.</i></dd>

          <dd>
            <h5>CONNECT(votent,que)</h5>
          </dd>

          <dd><i>La soci&eacute;t&eacute; qui lance l'OPA.</i></dd>

          <dd>
            <h5>CONNECT_REL(lance,qui)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COORDITEMS</h3>
      </li>

      <li style="list-style: none">
        This binary relation links coordinated elements. 

        <dl>
          <dt>Example</dt>

          <dd><i>Luc et Pierre participent au jeu.</i></dd>

          <dd>
            <h5>COORDITEMS(Luc,Pierre)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COREF</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a (relative) pronoun to its
        antecedent. In the current version of the parser, only
        subject relative pronouns are handled. 

        <dl>
          <dt>Example</dt>

          <dd><i>L'homme qui a lib&eacute;r&eacute; l'Europe
          centrale.</i></dd>

          <dd>
            <h5>COREF_REL(homme,qui)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>DEEPOBJ</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a deep object to the verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Les chefs sont convi&eacute;s &agrave; une
          r&eacute;union.</i></dd>

          <dd>
            <h5>DEEPOBJ(convi&eacute;s,chefs)</h5>
          </dd>

          <dd><i>Les chefs viennent d'&ecirc;tre invit&eacute;s
          &agrave; une r&eacute;union.</i></dd>

          <dd>
            <h5>DEEPOBJ(invit&eacute;s,chefs)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>DEEPSUBJ</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a deep subject to the verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Certains pays sont tent&eacute;s par un statut de
          neutralit&eacute;.</i></dd>

          <dd>
            <h5>DEEPSUBJ(tent&eacute;s,statut)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>DETERM</h3>
      </li>

      <li style="list-style: none">
        This binary relation links a nominal head and a determiner 

        <dl>
          <dt>Example</dt>

          <dd><i>Ces trois filles</i></dd>

          <dd>
            <h5>DETERM(filles,ces)</h5>
          </dd>

          <dd><i>Sur la table</i></dd>

          <dd>
            <h5>DETERM(table,sur)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>NEGAT</h3>
      </li>

      <li style="list-style: none">
        This unary dependency marks negated verbs. 

        <dl>
          <dt>Example</dt>

          <dd><i>Cette m&eacute;thode ne suffit plus.</i></dd>

          <dd>
            <h5>NEGAT(suffit)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>NMOD</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a modifier to the noun it
        modifies. 

        <dl>
          <dt>Example</dt>

          <dd><i>Elle a conduit au paroxysme actuel.</i></dd>

          <dd>
            <h5>NMOD(paroxysme,actuel)</h5>
          </dd>

          <dd><i>Par de savantes manoeuvre.</i></dd>

          <dd>
            <h5>NMOD(manoeuvres,savantes)</h5>
          </dd>

          <dd><i>Pour r&eacute;pondre &agrave; des situations de
          crise.</i></dd>

          <dd>
            <h5>NMOD(situations,crise)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>OBJ</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a direct object to its verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Ils a &eacute;cart&eacute; ses
          adversaries.</i></dd>

          <dd>
            <h5>OBJ(&eacute;cart&eacute;,adversaries)</h5>
          </dd>

          <dd><i>Sans voir que cela exigeait de la
          vigilence.</i></dd>

          <dd>
            <h5>OBJ_PROPQUE(voir,exigeait)</h5>
          </dd>

          <dd><i>Il souhaite investir.</i></dd>

          <dd>
            <h5>OBJ(souhaite,investir)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>PREPOBJ</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a preposition to the noun or the
        verb it precedes. 

        <dl>
          <dt>Example</dt>

          <dd><i>Les acteurs de l'&eacute;poque intervenaient
          souvent.</i></dd>

          <dd>
            <h5>PREPOBJ(&eacute;poque,de)</h5>
          </dd>

          <dd><i>Ils essaient de ne pas intervenir.</i></dd>

          <dd>
            <h5>PREPOBJ(intervenir,de)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>REFLEX</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a reflexive pronoun to the verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Il s'est d&eacute;rob&eacute; aux
          questions.</i></dd>

          <dd>
            <h5>REFLEX(d&eacute;rob&eacute;,s')</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>SUBJ</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches the surface subject to the verb,
        including infinitive verbs. 

        <dl>
          <dt>Example</dt>

          <dd><i>Ils comptent le programmer fin juin.</i></dd>

          <dd>
            <h5>SUBJ(comptent,Ils)</h5>
          </dd>

          <dd>
            <h5>SUBJ(programmer,Ils)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>SUBJCLIT</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a subject clitic to its verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Cet espace est-il urbanis&eacute; ?</i></dd>

          <dd>
            <h5>SUBJCLIT(urbanis&eacute;,-il)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>VMOD</h3>
      </li>

      <li style="list-style: none">
        This dependency attaches a modifier of a verb to the verb
        itself. The modifier can be an indirect complement or an
        adjunct of the verb. 

        <dl>
          <dt>Example</dt>

          <dd><i>Ils vont &agrave; Paris.</i></dd>

          <dd>
            <h5>VMOD(vont,Paris)</h5>
          </dd>

          <dd><i>Ils travaillent pour une banque.</i></dd>

          <dd>
            <h5>VMOD(travaillent,banque)</h5>
          </dd>

          <dd><i>Il travaillent pour pouvoir s'amuser.</i></dd>

          <dd>
            <h5>VMOD(travaillent,pouvoir)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>The Dependency Features</h3>
      </li>

      <li style="list-style: none">
        A list of suffixes are sometimes added to the dependency
        labels (described above) in order to give more specific
        information about the syntactic relation. Here follows the
        detailed list of the possible suffixes (features). 

        <div style="margin-left: 2em">
          <dl>
            <dd>
              <h4>_COORD</h4>
            </dd>

            <dd>This features (specific to the SUBJ and OBJ
            relations) means that the dependent in the current
            relation is coordinated with another element (which is
            also a dependent in another dependency occurrence
            having the same label than the current one.)</dd>

            <dd>
              <h4>_DEF</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a definite article.</dd>

            <dd>
              <h4>_DEM</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a demonstrative
            pronoun.</dd>

            <dd>
              <h4>_IMPERSO</h4>
            </dd>

            <dd>This feature (specific to the SUBJ relation) means
            the dependent (i.e. the subject) is an impersonal
            subject.</dd>

            <dd>
              <h4>_INT</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a interrogative
            pronoun.</dd>

            <dd>
              <h4>_NUM</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a numeral.</dd>

            <dd>
              <h4>_PASSIVE</h4>
            </dd>

            <dd>This feature means that the verbal head of the
            dependency is in a passive form.</dd>

            <dd>
              <h4>_POSIT1</h4>
            </dd>

            <dd>This feature (specific to the NMOD and VMOD
            relations) indicates that the dependent is attached to
            the nearest head preceding it.</dd>

            <dd>
              <h4>_POSIT2</h4>
            </dd>

            <dd>This feature (specific to the NMOD and VMOD
            relations) indicates that the dependent is attached to
            the second head candidate preceding it (less reliable
            than POSIT1 relations).</dd>

            <dd>
              <h4>_POSIT3</h4>
            </dd>

            <dd>This feature (specific to the NMOD and VMOD
            relations) indicates that the dependent is attached to
            the third head candidate preceding it (less reliable
            than POSIT2 relations).</dd>

            <dd>
              <h4>_POSS</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a possessive pronoun.</dd>

            <dd>
              <h4>_PROPQUE</h4>
            </dd>

            <dd>This feature means that the dependent is a clause
            introduced with the grammatical word "que".</dd>

            <dd>
              <h4>_QUANT</h4>
            </dd>

            <dd>This feature means that the determiner (dependent)
            of the DETERM dependency is a quantifier (e.g.
            "tous").</dd>

            <dd>
              <h4>_REL</h4>
            </dd>

            <dd>This feature means that the verbal head of the
            dependency is the main verb of a relative clause.</dd>

            <dd>
              <h4>_SC</h4>
            </dd>

            <dd>This feature means that both elements of the
            COORDITEMS dependency are verbal heads of clauses.</dd>

            <dd>
              <h4>_SPRED</h4>
            </dd>

            <dd>This feature (specific to the OBJ relation)
            indicates that the object of the (copula) verb is a
            subject predicate.</dd>

            <dd>
              <h4>_SUBORD</h4>
            </dd>

            <dd>This feature (specific to the VMOD relation) means
            that the dependent (i.e. the verb modifier) is a
            subordinate clause.</dd>
          </dl>
        </div>
      </li>
    </ul>
    <a href="page1.jsp"> Back </a>
  </body>
</html>

