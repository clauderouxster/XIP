<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<html>
  <head>
    <meta name="generator" content="HTML Tidy, see www.w3.org">

    <title>Dependencies nomenclature of the XIP English Grammar</title>
  </head>

  <body>
    <a href="page1.jsp"> Back </a>
    <center>
      <h1>Dependencies nomenclature of the XIP English Grammar</h1>
    </center>
    <b>Author :</b> C. Hag&egrave;ge 

    <h1>Introduction</h1>
    The purpose of this document is to list the dependencies that
    are extracted by the XIP English grammar. A XIP dependency is a
    n-ary relation (unary, binary) that binds one or two nodes
    together from the chunk tree. Usually, dependencies apply on
    lexical nodes. Each dependency is named with a specific label.
    A dependency label denotes the grammatical relation that has
    been computed between the nodes that have been bound together. 

    <h1>List of dependencies</h1>
    Below is the list of dependencies extracted by the English XIP
    parser: 

    <ul>
      <li>
        <h3>AGENT</h3>
      </li>

      <li style="list-style: none">
        This binary dependency links the inflected auxiliary of a
        passive verbal form to its agent complement<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>The mouse was eaten by the cat</i></dd>

          <dd>
            <h5>AGENT(was,cat)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COMPAR</h3>
      </li>

      <li style="list-style: none">
        This dependency links a "than" (comparative) to a following
        adjective<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>He is more vigorous than nice</i></dd>

          <dd>
            <h5>COMPAR(than,nice)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COMPOUND</h3>
      </li>

      <li style="list-style: none">
        This dependency links a form of the verb "have" and
        "got"<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>I have got a friend.</i></dd>

          <dd>
            <h5>COMPOUND(have,got)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>CONNECTOR_COMPLTHAT</h3>
      </li>

      <li style="list-style: none">
        This dependency links the verb of a that-clause to the
        "that" introducing this clause.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>They said that the investigation
          succeeded.</i></dd>

          <dd>
            <h5>CONNECTOR_COMPLTHAT(succeeded,that)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>CONNECTOR_SUBORD</h3>
      </li>

      <li style="list-style: none">
        It links the verb of a subordinated clause (that is neither
        a relative, nor a that-clause) to the subordinated
        conjunction introducing this clause.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>I will stay at home because it is
          raining.</i></dd>

          <dd>
            <h5>CONNECTOR_SUBORD(is,because)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COORD</h3>
      </li>

      <li style="list-style: none">
        This binary relation links a coordination conjunction to
        one of the coordinated elements. All the coordinated
        elements of a coordinated expression have to be linked by a
        COORD dependency.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>He ate, played and drank.</i></dd>

          <dd>
            <h5>COORD(and,ate)</h5>
          </dd>

          <dd>
            <h5>COORD(and,played)</h5>
          </dd>

          <dd>
            <h5>COORD(and,drank)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>COORD_NEG</h3>
      </li>

      <li style="list-style: none">
        This binary relation links a coordination conjunction to
        one negated coordinated element.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>Pastels contain no cadmium or other
          pigments.</i></dd>

          <dd>
            <h5>COORD_NEG(or,cadmium)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>DETD</h3>
      </li>

      <li style="list-style: none">
        This binary relation links a nominal head and a
        determiner<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>These three girls</i></dd>

          <dd>
            <h5>DETD(girls,These)</h5>
          </dd>

          <dd><i>On the table</i></dd>

          <dd>
            <h5>DETD(table,the)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>HEAD</h3>
      </li>

      <li style="list-style: none">
        This is a binary relation relating the nucleus of some
        chunk with the chunk itself.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>The three girls</i></dd>

          <dd>
            <h5>HEAD(girls,The three girls)</h5>
          </dd>

          <dd><i>Really nice</i></dd>

          <dd>
            <h5>HEAD(nice,Really nice)</h5>
          </dd>

          <dd><i>On the table</i></dd>

          <dd>
            <h5>HEAD(table,On the table)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>IOBJ_POST</h3>
      </li>

      <li style="list-style: none">
        This dependency links a verb to an indirect object (that is
        not introduced by any preposition) on its right.<br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>He gave Mary some flowers.</i></dd>

          <dd>
            <h5>IOBJ_POST(gave,Mary)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3><a id="main">The MAIN*</a> suite</h3>
      </li>

      <li style="list-style: none">
        MAIN* is a list of unary dependencies that marks the main
        verbal element of the principal clause of the parsed
        sentence. It corresponds to the last element of the verbal
        chain. A list of suffixes added to the MAIN dependencies is
        available in order to type aspectual and temporal features
        of the main verb. Here follows the detailed list of the
        possible suffixes that can appear with a MAIN dependency.
        All the different suffixes can be combinde together.<br>
        <br>
         

        <dl>
          <dd>
            <h4>_INSIST</h4>
          </dd>

          <dd>This suffix is present on the MAIN dependency when
          the main verb is preceded by a DO-auxiliary.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>They do like French films.</i></dd>

              <dd>
                <h5>MAIN_INSIST(like)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_MODAL</h4>
          </dd>

          <dd>This suffix is present on the MAIN dependency when
          the main verb is preceded by a modal auxiliary.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>They should come.</i></dd>

              <dd>
                <h5>MAIN_MODAL(come)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_PASSIVE</h4>
          </dd>

          <dd>This suffix is present on the MAIN dependency when
          the main verb is in the passive form.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>The mouse was eaten by the cat.</i></dd>

              <dd>
                <h5>MAIN_PASSIVE(eaten)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_PERFECT</h4>
          </dd>

          <dd>This suffix is present on the MAIN dependency when
          the main verb is in perfect.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>They have eaten the cake.</i></dd>

              <dd>
                <h5>MAIN_PERFECT(eaten)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_PROGRESS</h4>
          </dd>

          <dd>This suffix is present when the main verb is in the
          progressive form</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>When are you going</i></dd>

              <dd>
                <h5>MAIN_PROGRESS(going)</h5>
              </dd>

              <dd>As we mentioned before, suffixes can combine, so,
              for a sentence like:<br>
              <br>
               <i>He should have been caught</i>.<br>
              <br>
               We obtain the following MAIN dependency:</dd>

              <dd>
                <h5>MAIN_MODAL_PERFECT_PASSIVE(caught)</h5>
              </dd>
            </dl>
            IMPORTANT NOTE: If the main verbs of a sentence are
            coordinated, the MAIN dependency marks the coordinator.
            

            <dl>
              <dt>Example</dt>

              <dd><i>Merrymakers tossed confettis, squawked horns
              and popped champagne corks.</i></dd>

              <dd>
                <h5>MAIN(and)</h5>
              </dd>
            </dl>
          </dd>
        </dl>
      </li>

      <li>
        <h3>The MOD* suite</h3>
      </li>

      <li style="list-style: none">
        This set of dependencies links a governor and any kind of
        complements or adjuncts attached to this governor. Note
        that the governor can be a verb, a noun, an adjective or an
        adverb. Note also that we do not make any distinction
        between complements and adjuncts. To the MOD dependencies
        we add three kinds suffixes: 

        <ul>
          <li>- position suffixes</li>

          <li>- syntactic suffixes</li>

          <li>- typing suffixes</li>
        </ul>
        <br>
         

        <ol>
          <li>
            <h3>Position suffixes</h3>
          </li>

          <li style="list-style: none">
            <ul>
              <li style="list-style: none">
                They express the relative position of the modifier
                considering the governor is modified. 

                <dl>
                  <dd>
                    <h4>_PRE</h4>
                  </dd>

                  <dd>This suffix expresses that the modifier is on
                  the left of the governor in the sentence.</dd>

                  <dd>
                    <dl>
                      <dt><i>Example</i></dt>

                      <dd><i>They develop human overhead
                      capital.</i></dd>

                      <dd>
                        <h5>MOD_PRE(capital,overhead)</h5>
                      </dd>

                      <dd>
                        <h5>MOD_PRE(capital,human)</h5>
                      </dd>

                      <dd><i>He always sleeps.</i></dd>

                      <dd>
                        <h5>MOD_PRE(sleeps,always)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_POST</h4>
                  </dd>

                  <dd>This suffix expresses that the modifier is on
                  the right of the governor in the sentence.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>The new version will combine index
                      with customized services.</i></dd>

                      <dd>
                        <h5>MOD_POST(combine,services)</h5>
                      </dd>
                    </dl>
                  </dd>
                </dl>
              </li>
            </ul>
          </li>

          <li>
            <h3>Syntactic suffixes</h3>
          </li>

          <li style="list-style: none">
            <ul>
              <li style="list-style: none">
                <dl>
                  <dd>These suffixes express the syntactic nature
                  of the modifier when the modifier is not nominal.
                  They are always preceded by a position suffix
                  (except the _NEG suffix, see below). Here follows
                  the list of these suffixes:</dd>

                  <dd>
                    <h4>_GERUND</h4>
                  </dd>

                  <dd>This suffix expresses the fact that the
                  modifier is an ING-form.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>He left, after having done the
                      job.</i></dd>

                      <dd>
                        <h5>MOD_POST_GERUND(left,done)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_NEG</h4>
                  </dd>

                  <dd>This suffix expresses the fact that the
                  modifier has a negative polarity.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>They do not work.</i></dd>

                      <dd>
                        <h5>MOD_NEG(work,not)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_SENTENCE_RELATIV</h4>
                  </dd>

                  <dd>This suffix expresses that the modifier is a
                  relative clause. In this case, the modifying
                  clause is represented by its main verb.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>The book that I"m reading is
                      interesting.</i></dd>

                      <dd>
                        <h5>
                        MOD_POST_SENTENCE_RELATIV(book,reading)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_SENTENCE</h4>
                  </dd>

                  <dd>This suffix expresses that the modifier is
                  sentential (excluding the case of relatives, see
                  above). Like with relative clauses, the
                  sentential modifier is represented by its main
                  verb.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>She left because he arrived.</i></dd>

                      <dd>
                        <h5>MOD_POST_SENTENCE(left,arrived)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_PROPER</h4>
                  </dd>

                  <dd>This suffix is used in chains of complex
                  proper nouns in order to link the last name to
                  other elements of the chain.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>John W. Smith left.</i></dd>

                      <dd>
                        <h5>MOD_PROPER(Smith,John)</h5>
                      </dd>

                      <dd>
                        <h5>MOD_PROPER(Smith,W.)</h5>
                      </dd>
                    </dl>
                  </dd>
                </dl>
              </li>
            </ul>
          </li>

          <li>
            <h3>Typing suffixes</h3>
          </li>

          <li style="list-style: none">
            <ul>
              <li style="list-style: none">
                <dl>
                  <dd>They correspond to a first attempt to type a
                  semantic nature of modifiers. This typing is not
                  exhaustively implemented in the current grammar
                  version. Here is the list of these suffixes.</dd>

                  <dd>
                    <h4>_AGENTIF</h4>
                  </dd>

                  <dd>It is added to the link between a modifying
                  ING-form and a nominal head, when this nominal
                  head is the agent of the action expressed by the
                  ING-form</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>Recessive host mutation affecting
                      virus multiplication was discussed in this
                      paper.</i></dd>

                      <dd>
                        <h5>
                        MOD_POST_AGENTIF(mutation,affecting)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_DURATION</h4>
                  </dd>

                  <dd>This suffix types a modifier link, when the
                  modifier expresses an idea of temporal
                  duration.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>He waited for hours.</i></dd>

                      <dd>
                        <h5>MOD_POST_DURATION(waited,hours)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_LOC</h4>
                  </dd>

                  <dd>This suffix types a modifier link, when the
                  modifier expresses an idea of spatial
                  location.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>He stayed in Paris.</i></dd>

                      <dd>
                        <h5>MOD_POST_LOC(stayed,Paris)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_MANNER</h4>
                  </dd>

                  <dd>This suffix types a modifier link when the
                  modifier is a manner adverb.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>She spoke gently.</i></dd>

                      <dd>
                        <h5>MOD_POST_MANNER(spoke,gently)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_QUANTITY</h4>
                  </dd>

                  <dd>This suffix is added to specify that the
                  modifier expresses a quantity. This is done only
                  when the modifier is a WH-expression.</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>How much did he pay "</i></dd>

                      <dd>
                        <h5>MOD_QUANTITY(pay,how much)</h5>
                      </dd>
                    </dl>
                  </dd>

                  <dd>
                    <h4>_TEMP</h4>
                  </dd>

                  <dd>This suffix is added to MOD* dependency when
                  the modifier is a temporal expression</dd>

                  <dd>
                    <dl>
                      <dt>Example</dt>

                      <dd><i>He left yesterday.</i></dd>

                      <dd>
                        <h5>MOD_POST_TEMP(left,yesterday)</h5>
                      </dd>
                    </dl>
                  </dd>
                </dl>
              </li>
            </ul>
          </li>
        </ol>
      </li>

      <li>
        <h3>The NUCL* suite</h3>
      </li>

      <li style="list-style: none">
        This is a binary dependency that links elements of the
        verbal chain. Different suffixes are available for this
        dependency.<br>
         

        <dl>
          <dd>
            <h4>_VLINK</h4>
          </dd>

          <dd>VLINK is the suffix that is added on a NUCL
          dependency when the two linked lexical units are elements
          of the same complex verbal chain. To the _VLINK suffix,
          the suffixes available for the <a href="#main">MAIN*</a>
          suite are also added.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>Caldwell"s resignation had been expected for
              some time.</i></dd>

              <dd>
                <h5>NUCL_VLINK_PERFECT(had,been)</h5>
              </dd>
            </dl>

            <dl>
              <dd>
                <h5>NUCL_VLINK_PASSIVE(been,expected)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_PARTICLE</h4>
          </dd>

          <dd>PARTICLE is the suffix that is added on VLINK to
          express the relation between a verbal form and a verbal
          particle.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>The Georgia legislature will wind up its 1961
              session Monday.</i></dd>

              <dd>
                <h5>NUCL_PARTICLE(wind,up)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_SUBJCOMPL</h4>
          </dd>

          <dd>NUCL_SUBJCOMPL links a copula and its complement</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>Despite the warning, there was a unanimous
              vote to enter a candidate, according to Republicans
              who attended.</i></dd>

              <dd>
                <h5>NUCL_SUBJCOMPL(was,vote)</h5>
              </dd>
            </dl>
          </dd>
        </dl>
      </li>

      <li>
        <h3>OBJ*</h3>
      </li>

      <li style="list-style: none">
        OBJ* is a binary relation between the last element of the
        verbal chain and its direct object. 

        <ul>
          <li>When the object is on the left of the verb we have
          the dependency OBJ_PRE.</li>

          <li>When the object is on the right of the verb, we have
          the dependency OBJ_POST.</li>
        </ul>
        <br>
         

        <dl>
          <dt>Example</dt>

          <dd><i>I gave Mary some flowers.</i></dd>

          <dd>
            <h5>OBJ_POST(gave,flowers)</h5>
          </dd>

          <dd><i>What did you give her?</i></dd>

          <dd>
            <h5>OBJ_PRE(give,What)</h5>
          </dd>
        </dl>
      </li>

      <li style="list-style: none">
        <dl>
          <dd>Other suffixes can be added to this dependency. Here
          follows the list:</dd>
        </dl>

        <dl>
          <dd>
            <dl>
              <dd>
                <h4>_SENTENCE</h4>
              </dd>

              <dd>It marks that the object is sentential. It is
              represented by the verb of the object clause.</dd>

              <dd>
                <dl>
                  <dt>Example</dt>

                  <dd><i>He said that she would come.</i></dd>

                  <dd>
                    <h5>OBJ_POST_SENTENCE(said,come)</h5>
                  </dd>
                </dl>
              </dd>

              <dd>
                <h4>_GERUND</h4>
              </dd>

              <dd>It marks that the object is a verbal
              ING-form.</dd>

              <dd>
                <dl>
                  <dt>Example</dt>

                  <dd><i>He went fishing.</i></dd>

                  <dd>
                    <h5>OBJ_POST_GERUND(went,fishing)</h5>
                  </dd>
                </dl>
              </dd>

              <dd>
                <h4>_INFINIT</h4>
              </dd>

              <dd>This suffix expresses the fact that the object is
              an infinitive form.</dd>

              <dd>
                <dl>
                  <dt>Example</dt>

                  <dd><i>I like to play.</i></dd>

                  <dd>
                    <h5>OBJ_POST_INFINIT(like,play)</h5>
                  </dd>
                </dl>
              </dd>

              <dd>
                <h4>_NEG</h4>
              </dd>

              <dd>This suffix expresses the fact that the object is
              negated.</dd>

              <dd>
                <dl>
                  <dt>Example</dt>

                  <dd><i>They have no choice.</i></dd>

                  <dd>
                    <h5>OBJ_POST_NEG(have,choice)</h5>
                  </dd>
                </dl>
              </dd>

              <dd>
                <h4>_RELATIV</h4>
              </dd>

              <dd>This suffix expresses the fact that the object is
              a relative pronoun that has an object function within
              the relative clause.</dd>

              <dd>
                <dl>
                  <dt>Example</dt>

                  <dd><i>They have a book that I like to
                  read.</i></dd>

                  <dd>
                    <h5>OBJ_PRE_RELATIV(like,that)</h5>
                  </dd>
                </dl>
              </dd>
            </dl>
          </dd>
        </dl>
      </li>

      <li>
        <h3>OBJCOMPL_POST</h3>
      </li>

      <li style="list-style: none">This relation links a verb with
      an object complement<br>
      </li>

      <li style="list-style: none">
        <dl>
          <dt>Example</dt>

          <dd><i>They elected him president.</i></dd>

          <dd>
            <h5>OBJCOMPL_POST(elected,president)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>PREPD</h3>
      </li>

      <li style="list-style: none">This binary relation links a
      preposition to the nominal head of a PP.<br>
      </li>

      <li style="list-style: none">
        <dl>
          <dt>Example</dt>

          <dd><i>The book is on the table.</i></dd>

          <dd>
            <h5>PREPD(book,on)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>QUANTD</h3>
      </li>

      <li style="list-style: none">This binary relation links a
      quantifier and a nominal head.<br>
      </li>

      <li style="list-style: none">
        <dl>
          <dt>Example</dt>

          <dd><i>Some men arrived</i></dd>

          <dd>
            <h5>QUANTD(men,Some)</h5>
          </dd>

          <dd><i>We invited hundreds of people</i></dd>

          <dd>
            <h5>QUANTD(people,hundreds)</h5>
          </dd>
        </dl>
      </li>

      <li>
        <h3>SUBJ*</h3>
      </li>

      <li style="list-style: none">
        SUBJ* is a binary relation between the inflected element of
        the verbal chain and its subject.<br>
         When the subject is on the left of the verb we have the
        dependency SUBJ_PRE.<br>
         When the subject is on the right of the verb, we have the
        dependency SUBJ_POST.<br>
         Together with the position suffixes, we have a list of
        other suffixes added to the SUBJ dependency. 

        <dl>
          <dd>
            <h4>_GERUND</h4>
          </dd>

          <dd>This suffix is used when the subject is a
          ING-form</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>Going to the cinema is nice.</i></dd>

              <dd>
                <h5>SUBJ_PRE_GERUND(is,Going)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_NFINIT</h4>
          </dd>

          <dd>This suffix expresses the fact that the verb involved
          in this SUBJ dependency is an infinitive.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>I saw her come.</i></dd>

              <dd>
                <h5>SUBJ_PRE_NFINIT(come,her)</h5>
              </dd>
            </dl>
          </dd>

          <dd>
            <h4>_NEG</h4>
          </dd>

          <dd>This suffix is added to the SUBJ dependency when the
          subject is negated.</dd>

          <dd>
            <dl>
              <dt>Example</dt>

              <dd><i>No books are left.</i></dd>

              <dd>
                <h5>SUBJ_PRE_NEG(are,books)</h5>
              </dd>
            </dl>
          </dd>
        </dl>
      </li>
    </ul>
    <a href="page1.jsp"> Back </a>
  </body>
</html>

