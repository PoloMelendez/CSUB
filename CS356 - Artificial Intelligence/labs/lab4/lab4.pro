:- dynamic(environment/1).
:- dynamic(job/1).
:- dynamic(feedback/1).
ask( A ) :-
    write( 'Enter the value for '),
    write( A ),
    write( ': ' ),
    read( B ),
    Term =.. [A, B],
    asserta( Term ).

run :-  ask(environment),
        ask(job),
        ask(feedback),
        medium(X),
        write( 'Suggested medium: ' ),
        write(X).

stimulus_situation(verbal) :-   environment(papers);
                                environment(manuals);
                                environment(documents);
                                environment(textbooks).

stimulus_situation(visual) :-   environment(pictures);
                                environment(illustrations);
                                environment(photographs);
                                environment(diagrams).

stimulus_situation(physical_object) :-  environment(machines); 
                                        environment(buildings);
                                        environment(tools).

stimulus_situation(symbolic) :- environment(numbers);
                                environment(formulas);
                                environment(computer_programs).
                                
stimulus_response(oral) :-  job(lecture);
                            job(advising);
                            job(counseling).

stimulus_response(hands_on) :-  job(building);
                                job(repairing);
                                job(troubleshooting).

stimulus_response(documented) :-    job(writing);
                                    job(typing);
                                    job(drawing).

stimulus_response(analytical) :-    job(evaluating);
                                    job(reasoning);
                                    job(investigating).

medium(workshop) :- stimulus_situation(physical_object),
                    stimulus_response(hands_on),
                    feedback(required).

medium(lecture) :-  stimulus_situation(visual),
                    stimulus_response(analytical),
                    feedback(required).

medium(videocassette) :-    stimulus_situation(visual),
                            stimulus_response(documented),
                            feedback(not_required).

medium(lecture) :-  stimulus_situation(visual),
                    stimulus_response(oral),
                    feedback(required).

medium(lecture) :-  stimulus_situation(verbal),
                    stimulus_response(analytical),
                    feedback(required).

medium(roleplay) :- stimulus_situation(verbal),
                    stimulus_response(oral),
                    feedback(required).

